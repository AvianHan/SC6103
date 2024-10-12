// 航班相关服务实现
// flight service.c
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
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

extern Flight *flights;
extern int flight_count;

// 处理航班查询请求（通过出发地和目的地）
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    char source[50], destination[50];
    int found = 0;
    
    // 动态分配内存给 response
    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }
    memset(response, 0, response_size);

    // 从客户端请求中提取出发地和目的地
    sscanf(buffer, "QUERY_FLIGHT %s %s", source, destination);

    // 遍历航班数组，查找匹配的出发地和目的地
    for (int i = 0; i < flight_count; i++) {
        if (strcmp(flights[i].source_place, source) == 0 && strcmp(flights[i].destination_place, destination) == 0) {
            char flight_info[200];
            sprintf(flight_info, "Flight ID: %d\nMeal Option: %s\nBaggage Weight: %.2f kg\n",
                    flights[i].flight_id, flights[i].meal_option, flights[i].baggage_weight);

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

    // 将查询结果发送回客户端
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 释放动态分配的内存
    free(response);
}


// 处理航班详细信息查询请求（通过航班ID）
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id;
    sscanf(buffer, "QUERY_DETAILS %d", &flight_id);

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }

    int found = 0;
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            sprintf(response, "Flight ID: %d\nSource: %s\nDestination: %s\nDeparture Time: %d-%02d-%02d %02d:%02d\nAirfare: %.2f\nAvailable Seats: %d\nMeal Option: %s\nBaggage Weight: %.2f kg\n",
                    flights[i].flight_id, flights[i].source_place, flights[i].destination_place,
                    flights[i].departure_time.year, flights[i].departure_time.month, flights[i].departure_time.day,
                    flights[i].departure_time.hour, flights[i].departure_time.minute,
                    flights[i].airfare, flights[i].seat_availability,
                    flights[i].meal_option, flights[i].baggage_weight);
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Flight not found");
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    free(response);
}

// 处理航班座位预订请求
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id, seats;
    sscanf(buffer, "RESERVE %d %d", &flight_id, &seats);

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }

    int found = 0;
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            if (flights[i].seat_availability >= seats) {
                flights[i].seat_availability -= seats;
                sprintf(response, "Reservation successful. Remaining seats: %d\n", flights[i].seat_availability);
            } else {
                strcpy(response, "Not enough available seats");
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Flight not found");
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    free(response);
}