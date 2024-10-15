// 航班相关服务实现
// flight_service.c
#include <stdint.h>  // 添加这个头文件来定义 uint8_t 和 uint32_t
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#define BUFFER_SIZE 1024
#define MAX_LUGGAGE 400

// 定义月份名称的数组
const char *months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

// 处理航班查询请求（通过出发地和目的地）(已改动)
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    char source[50], destination[50];
    int found = 0;

    // 从请求中提取出发地和目的地
    sscanf(request, "query_flight_id %s %s", source, destination);
    printf("Received query: source=%s, destination=%s\n", source, destination);

    // 构建SQL查询语句
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT flight_id FROM flights WHERE source_place='%s' AND destination_place='%s'",
             source, destination);

    // 执行SQL查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Database query failed.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Database error occurred.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    // 动态分配内存给 response
    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        mysql_free_result(res);
        return;
    }
    int response_size = BUFFER_SIZE;
    memset(response, 0, response_size);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        char flight_info[100];
        snprintf(flight_info, sizeof(flight_info), "Flight ID: %s\n", row[0]);

        // 检查 response 缓冲区是否足够大，动态扩展
        if (strlen(response) + strlen(flight_info) >= response_size) {
            response_size *= 2;  // 扩展为原来的两倍
            response = (char *)realloc(response, response_size * sizeof(char));
            if (response == NULL) {
                perror("Memory reallocation failed");
                mysql_free_result(res);
                return;
            }
        }

        strcat(response, flight_info);  // 将找到的航班信息附加到响应中
        found++;
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        strcpy(response, "No flights found.\n");
    }

    // 发送响应到客户端
    ssize_t sent_len = sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    if (sent_len < 0) {
        perror("Failed to send response");
    } else {
        printf("Response sent to client: %s\n", response);
    }

    // 释放动态分配的内存和查询结果
    free(response);
    mysql_free_result(res);
}


// 查询航班的函数，使用数据库查询航班信息(已改动)
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    int flight_id;
    int found = 0;
    char query[256];
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);  // 确保缓冲区清空

    // 从客户端请求中提取航班ID
    sscanf(request, "query_flight_info %d", &flight_id);
    printf("Received query: flight_id=%d\n", flight_id);

    // 构建SQL查询语句
    snprintf(query, sizeof(query), "SELECT flight_id, source_place, destination_place, departure_year, departure_month, departure_day, departure_hour, departure_minute, airfare, seat_availability, baggage_availability FROM flights WHERE flight_id = %d", flight_id);

    // 执行SQL查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database query failed.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database error occurred.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_ROW row;

    // 遍历查询结果
    while ((row = mysql_fetch_row(res))) {
        char departure_time[100];

        // 格式化航班的出发时间为：Month day, year hour:minute
        snprintf(departure_time, sizeof(departure_time), "%s %02d, %d %02d:%02d",
                 months[atoi(row[4]) - 1],  // 通过月份数组找到月份名称
                 atoi(row[5]),               // day
                 atoi(row[3]),               // year
                 atoi(row[6]),               // hour
                 atoi(row[7]));              // minute

        // 格式化航班的全部信息，包括优化后的时间显示
        snprintf(response + strlen(response), sizeof(response) - strlen(response),
                 "Flight ID: %s\n"
                 "Source: %s\n"
                 "Destination: %s\n"
                 "Departure Time: %s\n"
                 "Airfare: %s\n"
                 "Seats Available: %s\n"
                 "Baggage Availability: %s kg\n\n",
                 row[0],  // flight_id
                 row[1],  // source_place
                 row[2],  // destination_place
                 departure_time,  // 格式化后的出发时间
                 row[8],  // airfare
                 row[9], // seat_availability
                 row[10]  // baggage_availability
        );

        found = 1;
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        snprintf(response, sizeof(response), "Flight not found.\n");
    }
    store_in_history(client_addr, request, response);
    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 释放查询结果
    mysql_free_result(res);

    // 输出生成的响应
    printf("Response sent to client: %s\n", response);
}

void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    int flight_id, seats;
    char query[256];
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID和要预定的座位数
    sscanf(request, "make_seat_reservation %d %d", &flight_id, &seats);
    printf("Received reservation request: Flight ID=%d, Seats=%d\n", flight_id, seats);

    // 构建SQL查询，检查航班的剩余座位数
    snprintf(query, sizeof(query), "SELECT seat_availability FROM flights WHERE flight_id=%d", flight_id);

    // 执行SQL查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database query failed.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database error occurred.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        int seat_availability = atoi(row[0]);  // 获取座位可用数量
        if (seat_availability == 0) {
            // 如果没有可用座位
            strcpy(response, "Reservation failed: No seats available.\n");
        } else if (seat_availability < seats) {
            // 如果座位不够
            strcpy(response, "Reservation failed: Not enough seats available. Reduce your reservation.\n");
        } else {
            // 更新座位数量
            int new_seat_availability = seat_availability - seats;
            snprintf(query, sizeof(query),
                     "UPDATE flights SET seat_availability=%d WHERE flight_id=%d",
                     new_seat_availability, flight_id);

            if (mysql_query(conn, query)) {
                fprintf(stderr, "UPDATE error: %s\n", mysql_error(conn));
                snprintf(response, sizeof(response), "Database update failed.\n");
            } else {
                // 返回预定确认信息
                snprintf(response, sizeof(response),
                         "Reservation confirmed for Flight ID: %d\nSeats remaining: %d\n",
                         flight_id, new_seat_availability);
            }
        }
    } else {
        // 如果没有找到匹配的航班
        strcpy(response, "Flight not found.\n");
    }

    // 释放查询结果
    mysql_free_result(res);
    store_in_history(client_addr, request, response);
    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 输出日志
    printf("Response sent to client: %s\n", response);
}


void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    int flight_id, baggages;
    char query[256];
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID和行李数量
    sscanf(request, "add_baggage %d %d", &flight_id, &baggages);
    printf("Received baggage reservation request: Flight ID=%d, Baggages=%d\n", flight_id, baggages);

    // 构建SQL查询语句，检查航班的行李可用性
    snprintf(query, sizeof(query), "SELECT baggage_availability FROM flights WHERE flight_id=%d", flight_id);

    // 执行SQL查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database query failed.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database error occurred.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        int baggage_availability = atoi(row[0]);  // 获取行李可用数量
        if (baggage_availability == 0) {
            // 如果没有可用的行李位置
            strcpy(response, "Baggage reservation failed: No baggage space available.\n");
        } else if (baggage_availability < baggages) {
            // 如果行李空间不足
            strcpy(response, "Baggage reservation failed: Not enough space for baggage. Reduce your request.\n");
        } else {
            // 更新行李可用数量
            int new_baggage_availability = baggage_availability - baggages;
            snprintf(query, sizeof(query),
                     "UPDATE flights SET baggage_availability=%d WHERE flight_id=%d",
                     new_baggage_availability, flight_id);

            if (mysql_query(conn, query)) {
                fprintf(stderr, "UPDATE error: %s\n", mysql_error(conn));
                snprintf(response, sizeof(response), "Database update failed.\n");
            } else {
                // 返回行李预定确认信息
                snprintf(response, sizeof(response),
                         "Baggage reservation confirmed for Flight ID: %d\nBaggage space remaining: %d\n",
                         flight_id, new_baggage_availability);
            }
        }
    } else {
        // 如果没有找到匹配的航班
        strcpy(response, "Flight not found.\n");
    }

    // 释放查询结果
    mysql_free_result(res);
    //store_in_history(sockfd, request, response);
    //store_in_history(&client_addr, request, response);
    store_in_history(client_addr, request, response);
    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 输出日志
    printf("Response sent to client: %s\n", response);
}

void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    int flight_id;
    char query[256];
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID
    sscanf(request, "query_baggage_availability %d", &flight_id);
    printf("Received query for baggage availability: Flight ID=%d\n", flight_id);

    // 构建SQL查询语句，查询航班的行李可用空间
    snprintf(query, sizeof(query), "SELECT baggage_availability FROM flights WHERE flight_id = %d", flight_id);

    // 执行SQL查询
    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database query failed.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        snprintf(response, sizeof(response), "Database error occurred.\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        // 获取航班行李可用空间并生成响应
        snprintf(response, sizeof(response), "Flight ID: %d\nBaggage space available: %s\n", flight_id, row[0]);
    } else {
        // 如果没有找到匹配的航班，返回错误消息
        strcpy(response, "Flight not found.\n");
    }

    // 释放查询结果
    mysql_free_result(res);
    store_in_history(client_addr, request, response);
    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 输出日志
    printf("Response sent to client: %s\n", response);
}
