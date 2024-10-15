// handleRequest.c
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>


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

// 处理请求的函数
void handleRequest(char *request, struct sockaddr_in cliaddr, int sockfd, socklen_t len, MYSQL *conn) {
    char response[1024];

    // 解析客户端的请求，根据命令类型进行不同的处理
    if (strncmp(request, "test_connection", 15) == 0) {
        printf("Received test connection request from client\n");
        strcpy(response, "Connection OK");
    } else if (strncmp(request, "query_flight_id", 15) == 0) {
        // 示例: 处理查询航班ID的逻辑
        printf("Received query_flight_id request\n");
        handle_query_flight(sockfd, &cliaddr, request, conn);
        // strcpy(response, "Flight ID list: 123, 456, 789");
    } else if (strncmp(request, "query_flight_info", 17) == 0) {
        // 示例: 处理查询航班信息的逻辑
        printf("Received query_flight_info request\n"); 
        // 在此处查询数据库
        handle_query_details(sockfd, &cliaddr, request, conn);
        // strcpy(response, "Flight info: Flight 123, Departure: 10:00, Seats: 100");
    } else if (strncmp(request, "make_seat_reservation", 21) == 0) {
        // 示例: 处理预订座位的逻辑
        printf("Received make_seat_reservation request\n");
        handle_reservation(sockfd, &cliaddr, request, conn);
        //strcpy(response, "Seat reservation confirmed");
    } else if (strncmp(request, "query_baggage_availability", 26) == 0) {
        // 示例: 处理查询行李信息的逻辑
        printf("Received query_baggage_availability request\n");
        handle_query_baggage_availability(sockfd, &cliaddr, request, conn);
        //strcpy(response, "Baggage availability: 50");
    } else if (strncmp(request, "add_baggage", 11) == 0) {
        // 示例: 处理添加行李的逻辑
        printf("Received add_baggage request\n");
        handle_add_baggage(sockfd, &cliaddr, request, conn);
        //strcpy(response, "Baggage added successfully");
    } else if (strncmp(request, "follow_flight_id", 16) == 0) {
        // 示例: 处理callback
        printf("Received follow_flight_id request\n");// 解析 follow_flight_id 请求
        int flight_id;
        sscanf(request, "follow_flight_id %d", &flight_id);
        // 注册该客户端监控航班
        printf("Received follow_flight_id request for flight_id: %d\n", flight_id);
        register_flight_monitor(sockfd, &cliaddr, flight_id);

        // 启动一个监控航班座位可用性的线程
        pthread_t monitor_thread;
        struct client_data *data = malloc(sizeof(struct client_data));
        data->sockfd = sockfd;
        data->client_addr = cliaddr;
        data->conn = conn;  // 传入数据库连接

        if (pthread_create(&monitor_thread, NULL, monitor_flights, (void *)data) != 0) {
            perror("Failed to create monitor thread");
        }
        pthread_detach(monitor_thread);  // 分离线程，使其在完成后自动释放资源

        strcpy(response, "Flight monitoring started.\n");
    } else {
        // 未知命令
        printf("Unknown command received: %s\n", request);
        strcpy(response, "Unknown command");
        sendto(sockfd, response, strlen(response), 0, (const struct sockaddr *)&cliaddr, len);
    }

    // 发送响应给客户端
    printf("Response sent to client: %s\n", response);
}

