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
#include <fcntl.h> 
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#endif
#include <pthread.h>
#include "server.h"
#include "communication.h" // 包含 marshalling 和 unmarshalling 功能
#include <mysql/mysql.h>
#include <fcntl.h> // for setting non-blocking mode

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_HISTORY 100
#define SERVER_IP "172.20.10.10" // server IP


Flight *flights = NULL;  // 初始化为 NULL
int flight_count = 0;
int max_flights = 100;

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

/*
struct client_data
{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    int sockfd;
    socklen_t addr_len;
     MYSQL *conn; // 新增：数据库连接
};

*/


// 设置套接字为非阻塞模式的函数
void set_nonblocking(int sockfd) {
#ifdef _WIN32
    // Windows 平台设置非阻塞模式
    u_long mode = 1;  // 1 表示开启非阻塞模式
    ioctlsocket(sockfd, FIONBIO, &mode);
#else
    // Linux/Unix 平台设置非阻塞模式
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL failed");
        return;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL failed");
    }
#endif
}

// ---- 1. Store history: 用于将处理过的请求和响应存储到历史记录中 ----
void store_in_history(struct sockaddr_in *client_addr, const char *request, const char *response)
{
    if (history_count < MAX_HISTORY)
    {
        history[history_count].client_addr = *client_addr;
        strncpy(history[history_count].request, request, BUFFER_SIZE);
        strncpy(history[history_count].response, response, BUFFER_SIZE);
        history_count++;
    }
    else
    {
        // FIFO: 删除最早的记录，腾出空间
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history[MAX_HISTORY - 1].client_addr = *client_addr;
        strncpy(history[MAX_HISTORY - 1].request, request, BUFFER_SIZE);
        strncpy(history[MAX_HISTORY - 1].response, response, BUFFER_SIZE);
    }
    
}

// ---- 2. Filter duplicate: 用于检查请求是否已经处理过，避免重复处理 ----
int find_in_history(int sockfd, struct sockaddr_in *client_addr, const char *request, char *response)
{
    for (int i = 0; i < history_count; i++)
    {
        if (strcmp(history[i].request, request) == 0 &&
            history[i].client_addr.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            history[i].client_addr.sin_port == client_addr->sin_port)
        {
            strcpy(response, history[i].response);
            printf("request duplicated! exit now.\n");
            //sendto
            sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
            return 1; // 请求已经处理过
        }
    }
    printf("request goes further ......\n");
    return 0; // 没有找到请求
}

// 处理客户端请求的线程
void *handle_client(void *arg)
{

    struct client_data *data = (struct client_data *)arg;
    char reply[BUFFER_SIZE];

    // 使用传入的数据库连接
    MYSQL *conn = data->conn;  // 现在可以在这个线程中使用 conn

    printf("handle_client: transfer into handleRequest!\n");
    if (use_at_least_once)
    {
        // At-least-once: 直接重新执行请求
        printf("Processing new request (At-least-once): %s\n", data->buffer);
        handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len, conn);

        // 生成新的响应
        snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);
    }
    else
    {
        // At-most-once: 检查历史记录，避免重复处理
        if (find_in_history(data->sockfd, &data->client_addr, data->buffer, reply))
        {
            // ---- 3. Re-reply: 找到重复请求，直接返回历史响应 ----
            printf("Duplicate request found (At-most-once), sending cached response.\n");

            snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);
            sendto(data->sockfd, reply, strlen(reply), 0, (struct sockaddr *)&data->client_addr, data->addr_len);
        }
        else
        {
            // 没有找到重复请求，处理客户端请求
            printf("Processing new request (At-most-once): %s\n", data->buffer);
            handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len, conn);

            // 生成新的响应
            snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);

            // 将请求和响应存储到历史记录中 (Store history)
            // store_in_history(&data->client_addr, data->buffer, reply);
        }
    }
    // 调用 handleRequest 函数处理接收到的请求
    // handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);
    printf("sending reply!\n");
    //发送响应给客户端（无论是新请求还是重复请求）


    //
    // 释放动态分配的内存
    free(data);

    return NULL;
}

int main(int argc, char *argv[])
{
    // 处理命令行参数
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

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif

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

    // 设置套接字为非阻塞模式
    set_nonblocking(sockfd);

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

    // 连接到数据库
    MYSQL *conn = connect_db();
    // 查询并显示航班数据
    // printf("Current flights:\n");
    query_flights(conn);

    printf("already connect to DB!\n");

    // 主循环：处理客户端请求
    while (1)
    {
        printf("Now we are dealing with a message...\n");
        memset(buffer, 0, BUFFER_SIZE);

        // 通过 select 来监控套接字可读性
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select error");
        } else if (activity == 0) {
            printf("No activity within the timeout period.\n");
            continue;
        }

        // 接收客户端请求
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("No data received yet.\n");
                continue;
            }
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
        data->conn = conn; // 将数据库连接传递给每个线程

        if (pthread_create(&client_thread, NULL, handle_client, (void *)data) != 0)
        {
            perror("Client thread creation failed");
            free(data);
        }
        pthread_detach(client_thread);
    }

#ifdef _WIN32
    WSACleanup();
#endif

    close(sockfd);
    free(buffer);

    // 关闭数据库连接
    close_db(conn);
    return 0;
}
