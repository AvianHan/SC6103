// 航班数据存储与管理
// data storage
#include "head.h"
#include <string.h>
#include <stdio.h>

#define MAX_FLIGHTS 100

Flight flights[MAX_FLIGHTS];
int flight_count = 0;

// 初始化航班信息
void initialize_flights() {
    // 示例航班数据
    flights[0] = (Flight){1, "Singapore", "Tokyo", "2024-10-12 08:00", 500.0, 50};
    flights[1] = (Flight){2, "Singapore", "New York", "2024-10-13 23:00", 1200.0, 30};
    flight_count = 2;
}

// 根据航班ID查找航班
Flight* find_flight_by_id(int flight_id) {
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            return &flights[i];
        }
    }
    return NULL;
}

// 更新航班的座位信息
int update_flight_seats(int flight_id, int seats) {
    Flight *flight = find_flight_by_id(flight_id);
    if (flight != NULL) {
        if (flight->available_seats >= seats) {
            flight->available_seats -= seats;
            return 1; // 更新成功
        } else {
            return -1; // 座位不足
        }
    }
    return 0; // 未找到航班
}

// 添加新航班
int add_flight(int flight_id, const char *source, const char *destination, const char *departure_time, float airfare, int available_seats) {
    if (flight_count >= MAX_FLIGHTS) {
        return -1; // 无法添加更多航班
    }
    flights[flight_count++] = (Flight){flight_id, "", "", "", airfare, available_seats};
    strncpy(flights[flight_count - 1].source, source, sizeof(flights[flight_count - 1].source) - 1);
    strncpy(flights[flight_count - 1].destination, destination, sizeof(flights[flight_count - 1].destination) - 1);
    strncpy(flights[flight_count - 1].departure_time, departure_time, sizeof(flights[flight_count - 1].departure_time) - 1);
    return 1; // 添加成功
}