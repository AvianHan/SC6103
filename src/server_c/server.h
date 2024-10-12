#ifndef SERVER_H
#define SERVER_H

// 通用包含
#if defined(__linux__) || defined(__APPLE__)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#elif defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif
#include <pthread.h>

//server.h

// 航班结构体
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
} DepartureTime;

typedef struct {
    int flight_id;
    char *source_place;
    char *destination_place;
    DepartureTime departure_time;
    float airfare;
    int seat_availability;
    char *meal_option;  // 固定几个选项，例如："Standard Meal", "Vegetarian Meal", "Seafood Meal", "Child Meal", "Halal Meal", "Diabetic Meal"
    float baggage_weight;  // 固定几个等级，例如：20.0, 30.0, 40.0, "Extra Purchase"
} Flight;

// 示例餐食选项
const char *meal_options[] = {
    "Standard Meal",
    "Vegetarian Meal",
    "Seafood Meal",
    "Child Meal",
    "Halal Meal",
    "Diabetic Meal"
};

// 示例行李重量等级
const float baggage_weights[] = {
    20.0,
    30.0,
    40.0,
    -1.0  // 表示额外购买
};

// 回调处理头文件
void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval);

// 数据存储头文件
void initialize_flights();
Flight* find_flight_by_id(int flight_id);
int update_flight_seats(int flight_id, int seats);
int add_flight(int flight_id, const char *source, const char *destination, DepartureTime departure_time, float airfare, int seat_availability);

// 航班服务头文件
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer);

// 线程池头文件
void thread_pool_init(int num_threads);
void thread_pool_add_task(void (*function)(void *), void *arg);
void thread_pool_destroy();

#endif // SERVER_H