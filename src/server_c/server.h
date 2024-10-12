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
#include "server.h"
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
    int baggage_availability;
} Flight;

// 回调处理头文件
void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval);
void handle_client_request(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void register_flight_monitor(int sockfd, struct sockaddr_in *client_addr, int flight_id);
void* monitor_flights(void* arg);

// 数据存储头文件
void initialize_flights();
Flight* find_flight_by_id(int flight_id);
int update_flight_seats(int flight_id, int seats);
int add_flight(int flight_id, const char *source, const char *destination, DepartureTime departure_time, float airfare, int seat_availability, int baggage_availability);


// 航班服务头文件
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *buffer);
// 线程池头文件
void thread_pool_init(int num_threads);
void thread_pool_add_task(void (*function)(void *), void *arg);
void thread_pool_destroy();

#endif // SERVER_H