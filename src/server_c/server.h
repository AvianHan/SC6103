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
#include <stdint.h>  // 添加这个来定义 uint8_t 和 uint32_t
#include <mysql/mysql.h>
#define BUFFER_SIZE 1024


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

struct client_data{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    int sockfd;
    socklen_t addr_len;
    MYSQL *conn; // 新增：数据库连接
};

extern Flight *flights;
extern int flight_count;
extern int max_flights;

// 回调处理头文件
// void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval);
void handle_client_request(int sockfd, struct sockaddr_in *client_addr, char *buffer, MYSQL *conn);
void register_flight_monitor(int sockfd, struct sockaddr_in *client_addr, int flight_id);
void* monitor_flights(void* arg);
// 在适当的头文件中声明
Flight* unmarshal_flight(const uint8_t* buffer, uint32_t* flight_data_length);


// 数据存储头文件
void initialize_flights();
Flight* find_flight_by_id(int flight_id);
int update_flight_seats(int flight_id, int seats);
int add_flight(int flight_id, const char *source, const char *destination, DepartureTime departure_time, float airfare, int seat_availability, int baggage_availability);


// 航班服务头文件

// Flight Service Function Declarations
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);

// void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer);
// void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *buffer);
// void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *buffer);
// void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer);
// void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
// void handle_add_baggage(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);
// void handle_query_baggage_availability(int sockfd, struct sockaddr_in *client_addr, char *request, MYSQL *conn);


// 线程池头文件
void thread_pool_init(int num_threads);
void thread_pool_add_task(void (*function)(void *), void *arg);
void thread_pool_destroy();

// 服务器文件
void handleRequest(char *request, struct sockaddr_in cliaddr, int sockfd, socklen_t len, MYSQL *conn);
void store_in_history(struct sockaddr_in* client_addr, const char* request, const char* response);
// int find_in_history(struct sockaddr_in* client_addr, const char* request, char* response);
int find_in_history(int sockfd, struct sockaddr_in *client_addr, const char *request, char *response);
void* handle_client(void* arg);




void set_nonblocking(int sockfd);

MYSQL* connect_db();
void close_db(MYSQL *conn);
void query_flights(MYSQL *conn);
void update_seats(MYSQL *conn, int flight_id, int seats_reserved);
void update_baggage(MYSQL *conn, int flight_id, int baggage_added);
void close_db(MYSQL *conn);

#endif // SERVER_H