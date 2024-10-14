// 航班数据存储与管理
// data_storage.c
#include <stdint.h>  // 添加这个头文件来定义 uint8_t 和 uint32_t
#include "server.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



// 初始化航班信息
void initialize_flights(int initial_capacity) {
    max_flights = initial_capacity;
    flights = (Flight *)malloc(max_flights * sizeof(Flight));
    if (flights == NULL) {
        perror("Memory allocation failed for flights");
        exit(EXIT_FAILURE);
    }

    // 示例航班数据，手动分配字符串以避免 strdup 失败风险
    flights[0].flight_id = 1;
    flights[0].source_place = malloc(strlen("Singapore") + 1);
    flights[0].destination_place = malloc(strlen("Tokyo") + 1);
    if (flights[0].source_place && flights[0].destination_place) {
        strcpy(flights[0].source_place, "Singapore");
        strcpy(flights[0].destination_place, "Tokyo");
        flights[0].departure_time = (DepartureTime){2024, 10, 12, 8, 0};
        flights[0].airfare = 500.0;
        flights[0].seat_availability = 50;
        flights[0].baggage_availability = 100;
    } else {
        perror("Memory allocation failed for flight strings");
        exit(EXIT_FAILURE);
    }

    flights[1].flight_id = 2;
    flights[1].source_place = malloc(strlen("Singapore") + 1);
    flights[1].destination_place = malloc(strlen("New York") + 1);
    if (flights[1].source_place && flights[1].destination_place) {
        strcpy(flights[1].source_place, "Singapore");
        strcpy(flights[1].destination_place, "New York");
        flights[1].departure_time = (DepartureTime){2024, 10, 13, 23, 0};
        flights[1].airfare = 1200.0;
        flights[1].seat_availability = 30;
        flights[1].baggage_availability = 50;
    } else {
        perror("Memory allocation failed for flight strings");
        exit(EXIT_FAILURE);
    }

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
        if (flight->seat_availability >= seats) {
            flight->seat_availability -= seats;
            return 1; // 更新成功
        } else {
            return -1; // 座位不足
        }
    }
    return 0; // 未找到航班
}

// 添加新航班
int add_flight(int flight_id, const char *source, const char *destination, DepartureTime departure_time, float airfare, int seat_availability, int baggage_availability) {
    if (flight_count >= max_flights) {
        max_flights *= 2;
        flights = (Flight *)realloc(flights, max_flights * sizeof(Flight));
        if (flights == NULL) {
            perror("Memory reallocation failed");
            return -1;
        }
    }

    flights[flight_count].flight_id = flight_id;

    // 动态分配并复制字符串
    flights[flight_count].source_place = malloc(strlen(source) + 1);
    flights[flight_count].destination_place = malloc(strlen(destination) + 1);
    if (flights[flight_count].source_place == NULL || flights[flight_count].destination_place == NULL) {
        perror("Memory allocation failed for flight strings");
        return -1;
    }
    strcpy(flights[flight_count].source_place, source);
    strcpy(flights[flight_count].destination_place, destination);

    flights[flight_count].departure_time = departure_time;
    flights[flight_count].airfare = airfare;
    flights[flight_count].seat_availability = seat_availability;
    flights[flight_count].baggage_availability = baggage_availability;
    flight_count++;

    return 1; // 添加成功
}

// 清理航班信息
void cleanup_flights() {
    if (flights != NULL) {
        for (int i = 0; i < flight_count; i++) {
            free(flights[i].source_place);
            free(flights[i].destination_place);
        }
        free(flights);
        flights = NULL;
    }
}

