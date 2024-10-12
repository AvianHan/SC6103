// 处理回调的实现
// callback_handler.c
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

// 注册客户端以监控航班座位可用性更新
void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval) {
    char response[BUFFER_SIZE];  // 使用栈上的内存而不是动态分配
    int found = 0;

    // 查找航班
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

    // 成功注册信息
    sprintf(response, "Registered for flight %d seat availability updates for %d seconds\n", flight_id, monitor_interval);
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    // 跟踪航班座位更新,第一次必定会存入当前空余位置因为空余位置必定不为-1
    int previous_seat_availability = -1;

    // 模拟在监控间隔内每隔一段时间向客户端发送座位更新
    for (int j = 0; j < monitor_interval; j++) {
        sleep(1);  // 假设每隔1秒检查一次更新

        // 检查航班座位是否有更新
        for (int i = 0; i < flight_count; i++) {
            if (flights[i].flight_id == flight_id) {
                if (flights[i].seat_availability != previous_seat_availability) {
                    previous_seat_availability = flights[i].seat_availability;

                    // 只有当座位情况发生变化时才发送更新
                    sprintf(response, "Flight %d updated seats available: %d\n", flight_id, flights[i].seat_availability);
                    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
                }
                break;
            }
        }
    }

    // 监控结束，停止更新
    strcpy(response, "Monitor interval expired, stopping updates\n");
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}
