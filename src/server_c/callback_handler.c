#include <stdio.h>
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

// callback_handler.c


#define BUFFER_SIZE 1024

// 注册的客户端结构体
typedef struct {
    struct sockaddr_in client_addr;
    int flight_id;
} ClientMonitor;

ClientMonitor client_monitors[100];  // 假设最多有100个客户端监控
int client_monitor_count = 0;

extern pthread_mutex_t flight_mutex;  // 互斥锁

// 处理客户端请求
void handle_client_request(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    char command[20];
    sscanf(buffer, "%s", command);

    uint32_t flight_data_length;
    // 通过 unmarshall 解析出 Flight 结构体
    Flight* flight = unmarshal_flight((uint8_t*)(buffer + strlen(command) + 1), &flight_data_length);

    if (strcmp(command, "QUERY_FLIGHT") == 0) {
        // 提取航班的出发地和目的地
        char* source = flight->source_place;
        char* destination = flight->destination_place;
        handle_query_flight(sockfd, client_addr, source, destination);
        free(flight->source_place);
        free(flight->destination_place);
    } else if (strcmp(command, "QUERY_FLIGHT_ID") == 0) {
        handle_query_details(sockfd, client_addr, buffer);
    } else if (strcmp(command, "RESERVE") == 0) {
        handle_reservation(sockfd, client_addr, buffer);
    } else if (strcmp(command, "ADD_BAGGAGE") == 0) {
        handle_add_baggage(sockfd, client_addr, buffer);
    } else if(strcmp(command, "QUERY_BAGGAGE") == 0) {
        handle_query_baggage_availability(sockfd, client_addr, buffer);
    } else if(strcmp(command, "MONITOR_FLIGHT") == 0) {
        int flight_id;
        sscanf(buffer, "MONITOR_FLIGHT %d", &flight_id);
        register_flight_monitor(sockfd, client_addr, flight_id);
    } else {
        char error_msg[BUFFER_SIZE];
        strcpy(error_msg, "Invalid command");
        sendto(sockfd, error_msg, strlen(error_msg), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    }    
    free(flight);  // 释放 Flight 结构体的内存
}

// 注册客户端监控航班
void register_flight_monitor(int sockfd, struct sockaddr_in *client_addr, int flight_id) {
    pthread_mutex_lock(&flight_mutex);

    // 注册客户端
    client_monitors[client_monitor_count].client_addr = *client_addr;
    client_monitors[client_monitor_count].flight_id = flight_id;
    client_monitor_count++;

    pthread_mutex_unlock(&flight_mutex);

    char response[BUFFER_SIZE];
    sprintf(response, "Registered for flight %d seat availability updates\n", flight_id);
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}

// 航班监控线程函数
void* monitor_flights(void* arg) {
    int sockfd = *(int*)arg;

    while (1) {
        pthread_mutex_lock(&flight_mutex);
        // 假设航班的 seat_availability 发生变化
        for (int i = 0; i < client_monitor_count; i++) {
            int flight_id = client_monitors[i].flight_id;
            // 这里应该检查 flight_id 对应的航班是否有座位更新
            // 如果更新，向客户端发送通知
            char response[BUFFER_SIZE];
            sprintf(response, "Flight %d updated seat availability\n", flight_id);
            sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_monitors[i].client_addr, sizeof(client_monitors[i].client_addr));
        }
        pthread_mutex_unlock(&flight_mutex);

        sleep(5);  // 每5秒检查一次航班状态
    }

    return NULL;
}
