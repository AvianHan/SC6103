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


// 处理航班查询请求（通过出发地和目的地）
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *source, char *destination) {
    int found = 0;
    
    // 动态分配内存给 response
    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }
    int response_size = BUFFER_SIZE;
    memset(response, 0, response_size);

    // 遍历航班数组，查找匹配的出发地和目的地
    for (int i = 0; i < flight_count; i++) {
        if (strcmp(flights[i].source_place, source) == 0 && strcmp(flights[i].destination_place, destination) == 0) {
            char flight_info[200];
            sprintf(flight_info, "Flight ID: %d\nBaggage Availability: %d kg\n",
                    flights[i].flight_id, flights[i].baggage_availability);

            // 检查 response 缓冲区是否足够大，动态扩展
            if (strlen(response) + strlen(flight_info) >= response_size) {
                response_size *= 2;  // 扩展为原来的两倍
                response = (char *)realloc(response, response_size * sizeof(char));
                if (response == NULL) {
                    perror("Memory reallocation failed");
                    return;
                }
            }

            strcat(response, flight_info);  // 将找到的航班信息附加到响应中
            found++;
        }
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        strcpy(response, "No flights found.\n");
    }

    // 确认响应字符串长度并且以 \0 结尾
    size_t response_len = strlen(response);
    if (response[response_len - 1] != '\n') {
        // 如果没有换行符，手动添加换行符
        strcat(response, "\n");
    }

    // 发送响应到客户端
    ssize_t sent_len = sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    if (sent_len < 0) {
        perror("Failed to send response");
    } else {
        printf("Response sent to client: %s\n", response);
    }
    printf("Response being sent to client: %s\n", response);

    // 释放动态分配的内存
    free(response);
}



// 查询航班的函数
// void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *request) {
//     int flight_id, found = 0;
//     char response[BUFFER_SIZE];  // 用于存储响应内容
//     memset(response, 0, BUFFER_SIZE);  // 确保缓冲区清空

//     // 从客户端请求中提取航班ID
//     sscanf(request, "query_flight_info %d", &flight_id);
//     printf("Received query: flight_id=%d\n", flight_id);

//     // 遍历航班数组，查找匹配的航班ID
//     for (int i = 0; i < flight_count; i++) {
//         printf("Checking flight %d...\n", flights[i].flight_id);

//         // 检查航班ID是否匹配
//         if (flights[i].flight_id == flight_id) {
//             char departure_time[100];  // 用于格式化时间
            
//             // 使用月份名称格式化航班的出发时间为：Month day, year hour:minute
//             snprintf(departure_time, sizeof(departure_time), "%s %02d, %d %02d:%02d",
//                      months[flights[i].departure_time.month - 1],  // 通过数组查找月份名称
//                      flights[i].departure_time.day,
//                      flights[i].departure_time.year,
//                      flights[i].departure_time.hour,
//                      flights[i].departure_time.minute);

//             // 格式化航班的全部信息，包括优化后的时间显示
//             snprintf(response, sizeof(response),
//                      "Flight ID: %d\n"
//                      "Source: %s\n"
//                      "Destination: %s\n"
//                      "Departure Time: %s\n"
//                      "Airfare: %.2f\n"
//                      "Seats Available: %d\n"
//                      "Baggage Availability: %d kg\n",
//                      flights[i].flight_id,
//                      flights[i].source_place,
//                      flights[i].destination_place,
//                      departure_time,  // 使用格式化后的时间
//                      flights[i].airfare,
//                      flights[i].seat_availability,
//                      flights[i].baggage_availability);

//             // 输出日志以供调试
//             printf("Matched flight info:\n%s\n", response);
//             found = 1;
//             break;  // 找到匹配的航班后退出循环
//         }
//     }

//     // 如果没有找到匹配的航班，返回错误消息
//     if (!found) {
//         snprintf(response, sizeof(response), "Flight not found.\n");
//     }

//     // 输出生成的响应
//     printf("Generated response: %s\n", response);

//     // 将结果发送回客户端
//     sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

//     // 日志输出，确认发送
//     printf("Response sent to client: %s\n", response);
// }

// 查询航班的函数，使用数据库查询航班信息
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn) {
    int flight_id, found = 0;
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
                 atoi(row[7]),               // hour
                 atoi(row[8]));              // minute

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
                 row[9],  // airfare
                 row[10], // seat_availability
                 row[11]  // baggage_availability
        );

        found = 1;
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        snprintf(response, sizeof(response), "Flight not found.\n");
    }

    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 释放查询结果
    mysql_free_result(res);

    // 输出生成的响应
    printf("Response sent to client: %s\n", response);
}


// 座位预定函数
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id, seats;
    int found = 0;
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID
    sscanf(buffer, "RESERVE %d %d", &flight_id, &seats);

    // 遍历航班数组，查找匹配的航班ID
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            found = 1;
            // 检查是否有空余座位
            if (flights[i].seat_availability == 0){
                // 如果没有空余座位，返回错误消息
                strcpy(response, "Reservation failed: No seats available.\n");
            }
            else if (flights[i].seat_availability < seats) {
                // 如果没有空余座位，返回错误消息
                strcpy(response, "Reservation failed: No enough seats available. Reduce your reservation.\n");
                
            } else {
                // 预定座位，减少可用座位数量
                flights[i].seat_availability -= seats;

                // 返回预定确认消息
                sprintf(response, "Reservation confirmed for Flight ID: %d\nSeats remaining: %d\n",
                        flights[i].flight_id,
                        flights[i].seat_availability);
            }
            break;  // 找到匹配的航班后退出循环
        }
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        strcpy(response, "Flight not found.\n");
    }

    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}

void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id, baggages;
    int found = 0;
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID
    sscanf(buffer, "ADD_BAGGAGE %d %d", &flight_id, &baggages);

     // 遍历航班数组，查找匹配的航班ID
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            found = 1;
            // 检查是否有空余行李位置
            if (flights[i].baggage_availability == 0){
                // 如果没有空余行李位置，返回错误消息
                strcpy(response, "Reservation failed: No baggage acceptable.\n");
            }
            else if (flights[i].baggage_availability < baggages) {
                // 如果没有空余行李位置，返回错误消息
                strcpy(response, "Reservation failed: No enough place for baggage available. Reduce your reservation.\n");
                
            } else {
                // 预定行李位置，减少可用座位数量
                flights[i].baggage_availability -= baggages;

                // 返回预定确认消息
                sprintf(response, "Reservation confirmed for Flight ID: %d\nBaggages still acceptable: %d\n",
                        flights[i].flight_id,
                        flights[i].baggage_availability);
            }
            break;  // 找到匹配的航班后退出循环
        }
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        strcpy(response, "Flight not found.\n");
    }

    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}


void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id;
    int found = 0;
    char response[BUFFER_SIZE];  // 用于存储响应内容
    memset(response, 0, BUFFER_SIZE);

    // 从客户端请求中提取航班ID
    sscanf(buffer, "QUERY_BAGGAGE %d", &flight_id);

    // 遍历航班数组，查找匹配的航班ID
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            found = 1;
            // 返回该航班的行李可用空间
            sprintf(response, "Flight ID: %d\nBaggage space available: %d\n", 
                    flights[i].flight_id, flights[i].baggage_availability);
            break;
        }
    }

    // 如果没有找到匹配的航班，返回错误消息
    if (!found) {
        strcpy(response, "Flight not found.\n");
    }

    // 将结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}
