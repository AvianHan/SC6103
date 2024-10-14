#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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
#include "communication.h" // 包含 marshalling 和 unmarshalling 功能
#include <mysql/mysql.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_HISTORY 100
#define SERVER_IP "172.20.10.10" // server IP


Flight *flights;  // 定义为指针，不指定大小
int flight_count = 0;  // 初始化航班计数
int max_flights = 100; // 假设最大航班数为 100


// 互斥锁
pthread_mutex_t flight_mutex = PTHREAD_MUTEX_INITIALIZER;

// 请求历史记录结构
typedef struct
{
    struct sockaddr_in client_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
} RequestHistory;

RequestHistory history[MAX_HISTORY];
int history_count = 0;
int use_at_least_once = 0; // Flag to toggle between at-least-once and at-most-once


// ---- 1. Store history: 用于将处理过的请求和响应存储到历史记录中 ----
void store_in_history(struct sockaddr_in *client_addr, const char *request, const char *response)
{
    if (history_count < MAX_HISTORY)
    {
        history[history_count].client_addr = *client_addr;
        strncpy(history[history_count].request, request, BUFFER_SIZE);
        strncpy(history[history_count].response, response, BUFFER_SIZE);
        history_count++;
        printf("Storing request: %s with response: %s\n", request, response);
        printf("store msg in history!\n");
    }
    else
    {
        printf("History is full, cannot store new requests.\n");
    }
}

// ---- 2. Filter duplicate: 用于检查请求是否已经处理过，避免重复处理 ----
int find_in_history(struct sockaddr_in *client_addr, const char *request, char *response)
{
    for (int i = 0; i < history_count; i++)
    {
        if (strcmp(history[i].request, request) == 0 &&
            history[i].client_addr.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            history[i].client_addr.sin_port == client_addr->sin_port)
        {
            strcpy(response, history[i].response);
            printf("request duplicated! exit now.\n");
            return 1; // 请求已经处理过
        }
    }
    printf("request goes further ......\n");
    return 0; // 没有找到请求
}

// 处理客户端请求的线程
void *handle_client(void *arg)
{
    struct client_data
    {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        int sockfd;
        socklen_t addr_len;
    };


    struct client_data *data = (struct client_data *)arg;
    char reply[BUFFER_SIZE];
    printf("handle_client: transfer into handleRequest!\n");
    if (use_at_least_once)
        {
            // At-least-once: 直接重新执行请求
            printf("Processing new request (At-least-once): %s\n", data->buffer);
            handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);

            // 生成新的响应
            snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);
        }
        else
        {
            // At-most-once: 检查历史记录，避免重复处理
            if (find_in_history(&data->client_addr, data->buffer, reply))
            {
                // ---- 3. Re-reply: 找到重复请求，直接返回历史响应 ----
                printf("Duplicate request found (At-most-once), sending cached response.\n");
            }
            else
            {
                // 没有找到重复请求，处理客户端请求
                printf("Processing new request (At-most-once): %s\n", data->buffer);
                handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);

                // 生成新的响应
                snprintf(reply, sizeof(reply), "Response to: %s", data->buffer);

                // 将请求和响应存储到历史记录中 (Store history)
                store_in_history(&data->client_addr, data->buffer, reply);
            }
        }
    // 调用 handleRequest 函数处理接收到的请求
    // handleRequest(data->buffer, data->client_addr, data->sockfd, data->addr_len);
    printf("sending reply!\n");
    sendto(data->sockfd, reply, strlen(reply), 0, (struct sockaddr *)&data->client_addr, data->addr_len);

    // 释放动态分配的内存
    free(data);

    return NULL;
}


// void* database_operations(void* arg) {
//     // 连接到数据库
//     MYSQL *conn = connect_db();

//     // 查询并显示航班数据
//     //printf("Current flights:\n");
//     query_flights(conn);

//     // 更新座位数（示例）
//     int flight_id = 1; // 示例航班ID
//     int seats_to_reserve = 2;
//     printf("\nUpdating seats for flight %d\n", flight_id);
//     update_seats(conn, flight_id, seats_to_reserve);

//     // 更新行李（示例）
//     int baggage_to_add = 10;
//     printf("\nUpdating baggage for flight %d\n", flight_id);
//     update_baggage(conn, flight_id, baggage_to_add);

//     // 关闭数据库连接
//     close_db(conn);

//     return NULL;
// }

void* database_operations(void* arg) {
    // 连接到数据库
    MYSQL *conn = connect_db();

    // 执行查询从数据库读取航班数据
    MYSQL_RES *result;
    MYSQL_ROW row;

    // 执行查询以获取航班信息
    if (mysql_query(conn, "SELECT flight_id, source_place, destination_place, departure_year, departure_month, departure_day, departure_hour, departure_minute, airfare, seat_availability, baggage_availability FROM flights")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    // 遍历结果并填充到全局航班数组
    flight_count = 0;
    while ((row = mysql_fetch_row(result))) {
        if (flight_count >= max_flights) {
            printf("Warning: Reached maximum number of flights.\n");
            break;
        }

        flights[flight_count].flight_id = atoi(row[0]);
        flights[flight_count].source_place = strdup(row[1]);
        flights[flight_count].destination_place = strdup(row[2]);

        // 填充 DepartureTime 结构体
        flights[flight_count].departure_time.year = atoi(row[3]);
        flights[flight_count].departure_time.month = atoi(row[4]);
        flights[flight_count].departure_time.day = atoi(row[5]);
        flights[flight_count].departure_time.hour = atoi(row[6]);
        flights[flight_count].departure_time.minute = atoi(row[7]);

        flights[flight_count].airfare = atof(row[8]);
        flights[flight_count].seat_availability = atoi(row[9]);
        flights[flight_count].baggage_availability = atoi(row[10]);

        flight_count++;
    }

    mysql_free_result(result);
    close_db(conn);
    return NULL;
}


int main(int argc, char *argv[]) {
    // 处理命令行参数
    if (argc != 2) {
        printf("Usage: %s [at-least-once | at-most-once]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "at-least-once") == 0) {
        use_at_least_once = 1;  // 设置标志为 at-least-once
        printf("Running with at-least-once fault tolerance.\n");
    } else if (strcmp(argv[1], "at-most-once") == 0) {
        use_at_least_once = 0;  // 设置标志为 at-most-once
        printf("Running with at-most-once fault tolerance.\n");
    } else {
        printf("Invalid argument. Use 'at-least-once' or 'at-most-once'.\n");
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    socklen_t addr_len = sizeof(client_addr);

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到指定端口
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    // 启动一个线程来执行数据库操作
    pthread_t db_thread;
    if (pthread_create(&db_thread, NULL, database_operations, NULL) != 0) {
        perror("Database thread creation failed");
        close(sockfd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // 主循环：处理客户端请求
    while (1) {
        printf("Now we are dealing with a message...\n");
        memset(buffer, 0, BUFFER_SIZE);

        // 接收客户端请求
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        // 为每个请求创建一个新的线程并传递给 handleRequest 函数处理
        pthread_t client_thread;
        struct client_data *data = malloc(sizeof(struct client_data));
        if (!data) {
            perror("Malloc failed");
            continue;
        }

        // 复制接收到的请求数据和客户端地址信息
        strncpy(data->buffer, buffer, BUFFER_SIZE);
        data->client_addr = client_addr;
        data->sockfd = sockfd;
        data->addr_len = addr_len;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)data) != 0) {
            perror("Client thread creation failed");
            free(data);
        }
        pthread_detach(client_thread);
    }

#ifdef _WIN32
    WSACleanup();
#endif

    // 等待数据库线程完成
    pthread_join(db_thread, NULL);

    close(sockfd);
    free(buffer);
    return 0;
}