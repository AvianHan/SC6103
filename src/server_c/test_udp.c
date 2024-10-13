#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080  // 服务器端口

void handleRequest(char *request, struct sockaddr_in cliaddr, int sockfd, socklen_t len) {
    char response[1024];
    
    // 解析客户端的请求，根据命令类型进行不同的处理
    if (strncmp(request, "test_connection", 15) == 0) {
        printf("Received test connection request from client\n");
        strcpy(response, "Connection OK");
    } else if (strncmp(request, "query_flight_id", 15) == 0) {
        // 示例: 处理查询航班ID的逻辑
        printf("Received query_flight_id request\n");
        // 假设查询成功，返回航班ID列表
        strcpy(response, "Flight ID list: 123, 456, 789");
    } else if (strncmp(request, "query_flight_info", 17) == 0) {
        // 示例: 处理查询航班信息的逻辑
        printf("Received query_flight_info request\n");
        // 假设查询成功，返回航班信息
        strcpy(response, "Flight info: Flight 123, Departure: 10:00, Seats: 100");
    } else if (strncmp(request, "make_seat_reservation", 21) == 0) {
        // 示例: 处理预订座位的逻辑
        printf("Received make_seat_reservation request\n");
        // 假设预订成功
        strcpy(response, "Seat reservation confirmed");
    } else if (strncmp(request, "query_baggage_availability", 26) == 0) {
        // 示例: 处理查询行李信息的逻辑
        printf("Received query_baggage_availability request\n");
        // 假设查询成功，返回行李信息
        strcpy(response, "Baggage availability: 50");
    } else if (strncmp(request, "add_baggage", 11) == 0) {
        // 示例: 处理添加行李的逻辑
        printf("Received add_baggage request\n");
        // 假设添加成功
        strcpy(response, "Baggage added successfully");
    } else {
        // 未知命令
        printf("Unknown command received: %s\n", request);
        strcpy(response, "Unknown command");
    }

    // 发送响应给客户端
    sendto(sockfd, response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
    printf("Response sent to client: %s\n", response);
}

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 绑定套接字到端口和IP地址
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;  // 监听所有IP地址
    servaddr.sin_port = htons(PORT);  // 监听端口

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    socklen_t len = sizeof(cliaddr);

    // 不断接收客户端消息并处理
    while (1) {
        int n = recvfrom(sockfd, buffer, 1024, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';  // 将接收的消息转为字符串
        printf("Received request: %s\n", buffer);

        // 处理请求
        handleRequest(buffer, cliaddr, sockfd, len);
    }

    close(sockfd);
    return 0;
}
