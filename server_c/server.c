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
#pragma comment(lib, "ws2_32.lib")
#endif
#include "flight_service.h"
#include "callback_handler.h"
#include "data_storage.h"
#include "thread_pool.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client_request(int sockfd, struct sockaddr_in *client_addr, char *buffer);

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
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
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
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    // 主循环：处理客户端请求
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // 接收客户端请求
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        // 处理客户端请求
        handle_client_request(sockfd, &client_addr, buffer);
    }

    #ifdef _WIN32
    WSACleanup();
    #endif

    close(sockfd);
    return 0;
}

// 处理客户端请求
void handle_client_request(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    char command[20];
    sscanf(buffer, "%s", command);

    if (strcmp(command, "QUERY_FLIGHT") == 0) {
        handle_query_flight(sockfd, client_addr, buffer);
    } else if (strcmp(command, "QUERY_DETAILS") == 0) {
        handle_query_details(sockfd, client_addr, buffer);
    } else if (strcmp(command, "RESERVE") == 0) {
        handle_reservation(sockfd, client_addr, buffer);
    } else {
        char *error_msg = "Invalid command";
        sendto(sockfd, error_msg, strlen(error_msg), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    }
}