// 航班数据存储与管理
// data storage.c
#include "server.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Flight *flights = NULL;
int flight_count = 0;
int max_flights = 0;

// 初始化航班信息
void initialize_flights(int initial_capacity) {
    max_flights = initial_capacity;
    flights = (Flight *)malloc(max_flights * sizeof(Flight));
    if (flights == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // 示例航班数据
    flights[0] = (Flight){1, strdup("Singapore"), strdup("Tokyo"), {2024, 10, 12, 8, 0}, 500.0, 50, strdup("Standard Meal"), 20.0};
    flights[1] = (Flight){2, strdup("Singapore"), strdup("New York"), {2024, 10, 13, 23, 0}, 1200.0, 30, strdup("Vegetarian Meal"), 30.0};
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
int add_flight(int flight_id, const char *source, const char *destination, DepartureTime departure_time, float airfare, int seat_availability, const char *meal_option, float baggage_weight) {
    if (flight_count >= max_flights) {
        max_flights *= 2;
        flights = (Flight *)realloc(flights, max_flights * sizeof(Flight));
        if (flights == NULL) {
            perror("Memory reallocation failed");
            return -1;
        }
    }

    flights[flight_count].flight_id = flight_id;
    flights[flight_count].source_place = strdup(source);
    flights[flight_count].destination_place = strdup(destination);
    flights[flight_count].departure_time = departure_time;
    flights[flight_count].airfare = airfare;
    flights[flight_count].seat_availability = seat_availability;
    flights[flight_count].meal_option = strdup(meal_option);
    flights[flight_count].baggage_weight = baggage_weight;
    flight_count++;

    return 1; // 添加成功
}

// 清理航班信息
void cleanup_flights() {
    if (flights != NULL) {
        for (int i = 0; i < flight_count; i++) {
            free(flights[i].source_place);
            free(flights[i].destination_place);
            free(flights[i].meal_option);
        }
        free(flights);
        flights = NULL;
    }
}