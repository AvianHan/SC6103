// 处理回调的实现
// callback_handler
#include "head.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

extern Flight flights[];
extern int flight_count;

// 注册客户端以监控航班座位可用性更新
void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval) {
    char response[BUFFER_SIZE];
    int found = 0;
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Flight not found");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        return;
    }

    sprintf(response, "Registered for flight %d seat availability updates for %d seconds\n", flight_id, monitor_interval);
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));

    // 模拟在监控间隔内每隔一段时间向客户端发送座位更新
    for (int j = 0; j < monitor_interval; j++) {
        sleep(1);  // 假设每隔1秒检查一次更新

        // 在实际实现中，应该检查航班座位是否有更新
        for (int i = 0; i < flight_count; i++) {
            if (flights[i].flight_id == flight_id) {
                char update_message[BUFFER_SIZE];
                sprintf(update_message, "Flight %d updated seats available: %d\n", flight_id, flights[i].available_seats);
                sendto(sockfd, update_message, strlen(update_message), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
                break;
            }
        }
    }

    strcpy(response, "Monitor interval expired, stopping updates\n");
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}