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
// void handleRequest(char *request, struct sockaddr_in cliaddr, int sockfd, socklen_t len, MYSQL *conn) {
//     char response[1024];

//     // 解析客户端的请求，根据命令类型进行不同的处理
//     if (strncmp(request, "test_connection", 15) == 0) {
//         printf("Received test connection request from client\n");
//         strcpy(response, "Connection OK");
//     } else if (strncmp(request, "query_flight_id", 15) == 0) {
//         // 示例: 处理查询航班ID的逻辑
//         printf("Received query_flight_id request\n");
//         handle_query_flight(sockfd, &cliaddr, request, conn);
//         // strcpy(response, "Flight ID list: 123, 456, 789");
//     } else if (strncmp(request, "query_flight_info", 17) == 0) {
//         // 示例: 处理查询航班信息的逻辑
//         printf("Received query_flight_info request\n"); 
//         // 在此处查询数据库
//         handle_query_details(sockfd, &cliaddr, request, conn);
//         // strcpy(response, "Flight info: Flight 123, Departure: 10:00, Seats: 100");
//     } else if (strncmp(request, "make_seat_reservation", 21) == 0) {
//         // 示例: 处理预订座位的逻辑
//         printf("Received make_seat_reservation request\n");
//         handle_reservation(sockfd, &cliaddr, request, conn);
//         //strcpy(response, "Seat reservation confirmed");
//     } else if (strncmp(request, "query_baggage_availability", 26) == 0) {
//         // 示例: 处理查询行李信息的逻辑
//         printf("Received query_baggage_availability request\n");
//         handle_query_baggage_availability(sockfd, &cliaddr, request, conn);
//         //strcpy(response, "Baggage availability: 50");
//     } else if (strncmp(request, "add_baggage", 11) == 0) {
//         // 示例: 处理添加行李的逻辑
//         printf("Received add_baggage request\n");
//         handle_add_baggage(sockfd, &cliaddr, request, conn);
//         //strcpy(response, "Baggage added successfully");
//     } else if (strncmp(request, "follow_flight_id", 16) == 0) {
//         // 示例: 处理添加行李的逻辑
//         printf("Received follow_flight_id request\n");
        
//         //strcpy(response, "Baggage added successfully");
//     } else {
//         // 未知命令
//         printf("Unknown command received: %s\n", request);
//         strcpy(response, "Unknown command");
//         sendto(sockfd, response, strlen(response), 0, (const struct sockaddr *)&cliaddr, len);
//     }

//     // 发送响应给客户端
//     // sendto(sockfd, response, strlen(response), 0, (const struct sockaddr *)&cliaddr, len);
//     printf("Response sent to client: %s\n", response);
// }

void handleRequest(char *request, struct sockaddr_in cliaddr, int sockfd, socklen_t len, MYSQL *conn) {
    char response[1024];
    uint8_t* buffer = (uint8_t*)request;
    size_t buffer_len = sizeof(buffer);
    size_t offset = 0;  // 用于追踪当前的读取位置

    // Step 1: 读取请求类型的长度（4 个字节）
    uint32_t request_type_len;
    if (buffer_len < 4) {
        fprintf(stderr, "Buffer too short!\n");
        return;
    }
    request_type_len = ntohl(*(uint32_t *)(buffer + offset));  // 将大端字节序转换为主机字节序
    offset += 4;

    if (offset + request_type_len > buffer_len) {
        fprintf(stderr, "Invalid buffer length for request type!\n");
        return;
    }

    // Step 2: 读取请求类型字符串
    char request_type[100];
    memcpy(request_type, buffer + offset, request_type_len);
    request_type[request_type_len] = '\0';  // 添加字符串结束符
    offset += request_type_len;

    printf("Request Type: %s\n", request_type);

    // Step 3: 根据请求类型解析后续参数
    if (strcmp(request_type, "test_connection") == 0) {
        // Test connection 不需要任何额外参数
        printf("Test connection: No additional parameters.\n");

    } else if (strcmp(request_type, "query_flight_id") == 0) {
        // query_flight_id 会传递两个字符串：source 和 destination
        uint32_t source_len, dest_len;
        
        // 读取 source 的长度
        source_len = ntohl(*(uint32_t *)(buffer + offset));  // 从大端到主机字节序
        offset += 4;
        char source[100];
        memcpy(source, buffer + offset, source_len);
        source[source_len] = '\0';
        offset += source_len;

        // 读取 destination 的长度
        dest_len = ntohl(*(uint32_t *)(buffer + offset));
        offset += 4;
        char destination[100];
        memcpy(destination, buffer + offset, dest_len);
        destination[dest_len] = '\0';
        offset += dest_len;

        printf("Source: %s, Destination: %s\n", source, destination);
        handle_query_flight(sockfd, &cliaddr, request, conn, source, destination);        

    } else if (strcmp(request_type, "query_flight_info") == 0) {
        // query_flight_info 会传递一个 int（航班 ID）
        if (offset + 4 > buffer_len) {
            fprintf(stderr, "Buffer too short for flight_id!\n");
            return;
        }
        uint32_t flight_id = ntohl(*(uint32_t *)(buffer + offset));  // 读取并转换航班 ID
        offset += 4;

        printf("Flight ID: %u\n", flight_id);
        handle_query_details(sockfd, &cliaddr, request, conn, flight_id);

    } else if (strcmp(request_type, "make_seat_reservation") == 0) {
        // make_seat_reservation 需要两个 int：航班 ID 和座位数
        if (offset + 8 > buffer_len) {
            fprintf(stderr, "Buffer too short for seat reservation!\n");
            return;
        }
        uint32_t flight_id = ntohl(*(uint32_t *)(buffer + offset));
        offset += 4;
        uint32_t seats = ntohl(*(uint32_t *)(buffer + offset));
        offset += 4;

        printf("Flight ID: %u, Seats: %u\n", flight_id, seats);
        handle_reservation(sockfd, &cliaddr, request, conn, flight_id, seats);

    } else if (strcmp(request_type, "query_baggage_availability") == 0) {
        // query_baggage_availability 需要一个 int（航班 ID）
        if (offset + 4 > buffer_len) {
            fprintf(stderr, "Buffer too short for baggage query!\n");
            return;
        }
        uint32_t flight_id = ntohl(*(uint32_t *)(buffer + offset));  // 读取并转换航班 ID
        offset += 4;

        printf("Flight ID: %u\n", flight_id);
        handle_query_baggage_availability(sockfd, &cliaddr, request, conn, flight_id);

    } else if (strcmp(request_type, "add_baggage") == 0) {
        // add_baggage 需要两个 int：航班 ID 和行李数量
        if (offset + 8 > buffer_len) {
            fprintf(stderr, "Buffer too short for baggage addition!\n");
            return;
        }
        uint32_t flight_id = ntohl(*(uint32_t *)(buffer + offset));
        offset += 4;
        uint32_t baggages = ntohl(*(uint32_t *)(buffer + offset));
        offset += 4;

        printf("Flight ID: %u, Baggage Count: %u\n", flight_id, baggages);
        handle_add_baggage(sockfd, &cliaddr, request, conn, flight_id, baggages);

    } else {
        printf("Unknown request type: %s\n", request_type);
    }
    // 发送响应给客户端
    // sendto(sockfd, response, strlen(response), 0, (const struct sockaddr *)&cliaddr, len);
    printf("Response sent to client: %s\n", response);
}
