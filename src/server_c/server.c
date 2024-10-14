#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif
#include <pthread.h>
#include "server.h"
#include "communication.h" // 包含 marshalling 和 unmarshalling 功能

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_HISTORY 100
#define SERVER_IP "172.20.10.10" // server IP

// 互斥锁
pthread_mutex_t flight_mutex = PTHREAD_MUTEX_INITIALIZER;

// 请求历史记录结构
typedef struct
{
    struct sockaddr_in client_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
} RequestHistory;

RequestHistory history[MAX_HISTORY];
int history_count = 0;
int use_at_least_once = 0; // Flag to toggle between at-least-once and at-most-once

// ---- 1. Store history: 用于将处理过的请求和响应存储到历史记录中 ----
void store_in_history(struct sockaddr_in *client_addr, const char *request, const char *response)
{
    if (history_count < MAX_HISTORY)
    {
        history[history_count].client_addr = *client_addr;
        strncpy(history[history_count].request, request, BUFFER_SIZE);
        strncpy(history[history_count].response, response, BUFFER_SIZE);
        history_count++;
        printf("store msg in history!\n");
    }
    else
    {
        printf("History is full, cannot store new requests.\n");
    }
}

// ---- 2. Filter duplicate: 用于检查请求是否已经处理过，避免重复处理 ----
int find_in_history(struct sockaddr_in *client_addr, const char *request, char *response)
{
    for (int i = 0; i < history_count; i++)
    {
        if (strcmp(history[i].request, request) == 0 &&
            history[i].client_addr.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            history[i].client_addr.sin_port == client_addr->sin_port)
        {
            strcpy(response, history[i].response);
            printf("request duplicated! exit now.\n");
            return 1; // 请求已经处理过
        }
    }
    printf("request goes further ......\n");
    return 0; // 没有找到请求
}

// 处理客户端请求的线程
void *handle_client(void *arg)
{
    struct client_data
    {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        int sockfd;
        socklen_t addr_len;
    };


    struct client_data *data = (struct client_data *)arg;
    char reply[BUFFER_SIZE];
    printf("handle_client: transfer into handleRequest!\n");
    if (use_at_least_once)
        {
            // At-least-once: 直接重新执行请求
            printf("Processing new request (At-least-once): %s\n", data->buffer);
            handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);

            // 生成新的响应
            snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);
        }
        else
        {
            // At-most-once: 检查历史记录，避免重复处理
            if (find_in_history(data->client_addr, data->buffer, reply))
            {
                // ---- 3. Re-reply: 找到重复请求，直接返回历史响应 ----
                printf("Duplicate request found (At-most-once), sending cached response.\n");
            }
            else
            {
                // 没有找到重复请求，处理客户端请求
                printf("Processing new request (At-most-once): %s\n", data->buffer);
                handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);

                // 生成新的响应
                snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);

                // 将请求和响应存储到历史记录中 (Store history)
                store_in_history(data->client_addr, data->buffer, reply);
            }
        }
    // 调用 handleRequest 函数处理接收到的请求
    // handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);
    printf("sending reply!\n");
    sendto(data->sockfd, reply, strlen(reply), 0, (struct sockaddr *)data->client_addr, data->addr_len);
    // 释放动态分配的内存
    free(data);

    return NULL;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif
    if (argc != 2)
    {
        printf("Usage: %s [at-least-once | at-most-once]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "at-least-once") == 0)
    {
        use_at_least_once = 1; // 设置标志为 at-least-once
        printf("Running with at-least-once fault tolerance.\n");
    }
    else if (strcmp(argv[1], "at-most-once") == 0)
    {
        use_at_least_once = 0; // 设置标志为 at-most-once
        printf("Running with at-most-once fault tolerance.\n");
    }
    else
    {
        printf("Invalid argument. Use 'at-least-once' or 'at-most-once'.\n");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    socklen_t addr_len = sizeof(client_addr);

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到指定端口
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    // 主循环：处理客户端请求
    while (1)
    {
        printf("now we are dealing msg......\n");
        memset(buffer, 0, BUFFER_SIZE);
        // char reply[BUFFER_SIZE];

        // 接收客户端请求
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            perror("Receive failed");
            continue;
        }

      
        // 为每个请求创建一个新的线程并传递给 handleRequest 函数处理
        pthread_t client_thread;
        struct client_data *data = malloc(sizeof(struct client_data));
        if (!data)
        {
            perror("Malloc failed");
            continue;
        }

        // 复制接收到的请求数据和客户端地址信息
        strncpy(data->buffer, buffer, BUFFER_SIZE);
        data->client_addr = client_addr;
        data->sockfd = sockfd;
        data->addr_len = addr_len;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)data) != 0)
        {
            perror("Client thread creation failed");
            free(data);
        }                                                                    
        printf("thread created! MSG goes in handle_client.\n");
        // char reply[BUFFER_SIZE];

        // 发送响应给客户端（无论是新请求还是重复请求）
        // printf("sending reply!\n");
        // sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&client_addr, addr_len);
        // 分离线程，使其可以自动释放资源，不阻塞主线程
        pthread_detach(client_thread);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    printf("gonna exit!\n");
    close(sockfd);
    free(buffer);
    return 0;
}
