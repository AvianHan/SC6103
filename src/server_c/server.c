#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
#include "handler.h"

#define PORT 8080
#define BUFFER_SIZE 1024

// 航班数据
pthread_mutex_t flight_mutex = PTHREAD_MUTEX_INITIALIZER;  // 互斥锁

void* handle_client(void* arg);    // 处理客户端请求的线程

int main() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
    #endif

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    socklen_t addr_len = sizeof(client_addr);

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到指定端口
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    // 启动航班监控线程
    pthread_t monitor_thread;
    if (pthread_create(&monitor_thread, NULL, monitor_flights, (void*)&sockfd) != 0) {
        perror("Monitor thread creation failed");
        close(sockfd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 主循环：处理客户端请求
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // 接收客户端请求
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        // 每次接收请求创建一个线程来处理客户端
        pthread_t client_thread;
        struct sockaddr_in* client_addr_copy = malloc(sizeof(struct sockaddr_in));
        memcpy(client_addr_copy, &client_addr, sizeof(struct sockaddr_in));  // 复制客户端地址
        if (pthread_create(&client_thread, NULL, handle_client, (void*)client_addr_copy) != 0) {
            perror("Client thread creation failed");
            free(client_addr_copy);
        }
    }

    #ifdef _WIN32
    WSACleanup();
    #endif

    close(sockfd);
    free(buffer);
    return 0;
}

// 处理客户端请求的线程
void* handle_client(void* arg) {
    int sockfd = *(int*)arg;
    struct sockaddr_in client_addr = *((struct sockaddr_in*)arg);
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 释放传入的参数内存
    free(arg);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        // 处理客户端请求
        handle_client_request(sockfd, &client_addr, buffer);
    }

    return NULL;
}
