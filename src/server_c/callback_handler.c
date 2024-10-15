#include <stdint.h> // 添加这个头文件来定义 uint8_t 和 uint32_t
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif


#include <pthread.h>
#include "server.h"

// callback_handler.c

#define BUFFER_SIZE 1024

// 注册的客户端结构体
typedef struct
{
    struct sockaddr_in client_addr;
    int flight_id;
    int seat_availability;
} ClientMonitor;

ClientMonitor client_monitors[100]; // 假设最多有100个客户端监控
int client_monitor_count = 0;

extern pthread_mutex_t flight_mutex; // 互斥锁

// 注册客户端监控航班
// void register_flight_monitor(int sockfd, struct sockaddr_in *client_addr, int flight_id)
// {
//     pthread_mutex_lock(&flight_mutex);

//     // 注册客户端
//     client_monitors[client_monitor_count].client_addr = *client_addr;
//     client_monitors[client_monitor_count].flight_id = flight_id;
//     client_monitor_count++;

//     pthread_mutex_unlock(&flight_mutex);

//     char response[BUFFER_SIZE];

//     // 打印注册客户端的 IP 地址和端口号
//     char client_ip[INET_ADDRSTRLEN];
//     inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
//     int client_port = ntohs(client_addr->sin_port);
//     printf("Client registered for flight %d seat updates from IP: %s, Port: %d\n", flight_id, client_ip, client_port);

//     sprintf(response, "Registered for flight %d seat availability updates\n", flight_id);
//     sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
// }

void register_flight_monitor(int sockfd, struct sockaddr_in *client_addr, int flight_id)
{
    pthread_mutex_lock(&flight_mutex);

    // 注册客户端
    client_monitors[client_monitor_count].client_addr = *client_addr;
    client_monitors[client_monitor_count].flight_id = flight_id;
    client_monitor_count++;

    pthread_mutex_unlock(&flight_mutex);

    // 向客户端发送注册成功的响应
    char response[BUFFER_SIZE];
    sprintf(response, "Registered for flight %d seat availability updates\n", flight_id);
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}
// 航班监控线程函数
// void *monitor_flights(void *arg)
// {

//     struct client_data *data = (struct client_data *)arg;
//     char reply[BUFFER_SIZE];

//     // 使用传入的数据库连接
//     // MYSQL *conn = data->conn;  // 现在可以在这个线程中使用 conn

//     // int sockfd = *(int*)arg;
//     MYSQL *conn = connect_db(); // 连接数据库

//     while (1)
//     {
//         pthread_mutex_lock(&flight_mutex);

//         // 遍历每个客户端监控的航班
//         for (int i = 0; i < client_monitor_count; i++)
//         {
//             int flight_id = client_monitors[i].flight_id;

//             // 查询当前航班的座位可用性
//             char query[256];
//             snprintf(query, sizeof(query), "SELECT seat_availability FROM flights WHERE flight_id = %d", flight_id);
//             printf("LOOP I:now we have %d client_monitors\n", client_monitor_count);
//             if (mysql_query(conn, query))
//             {
//                 fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
//                 continue;
//             }

//             MYSQL_RES *res = mysql_store_result(conn);
//             if (res == NULL)
//             {
//                 fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
//                 continue;
//             }

//             MYSQL_ROW row = mysql_fetch_row(res);
//             if (row)
//             {
//                 int current_seat_availability = atoi(row[0]);
//                 printf("LOOP J: now current_seat_availability is %d\n", current_seat_availability);

//                 // 比较当前座位可用数量是否变化
//                 for (int j = 0; j < client_monitor_count; j++)
//                 {
//                     if (client_monitors[j].flight_id == flight_id)
//                     {
//                         if (client_monitors[j].seat_availability != current_seat_availability)
//                         {
//                             client_monitors[j].seat_availability = current_seat_availability; // 更新状态
//                             printf("seats avail changed\n");
//                             // 通知注册监控该航班的客户端
//                             char response[BUFFER_SIZE];
//                             snprintf(response, sizeof(response),
//                                      "Flight %d seat availability updated to %d\n",
//                                      flight_id, current_seat_availability);

//                             // 将更新发送到注册的客户端
//                             // sendto(data->sockfd, response, strlen(response), 0,(struct sockaddr *)data->client_addr,sizeof(data->client_addr));
//                             ssize_t sent_len = sendto(data->sockfd, response, strlen(response), 0,
//                                                       (struct sockaddr *)&data->client_addr,
//                                                       sizeof(data->client_addr));

//                             // 判断 sendto 是否成功
//                             if (sent_len == -1)
//                             {
//                                 // 输出错误信息
//                                 perror("Failed to send data with sendto");
//                             }
//                             else
//                             {
//                                 // 输出成功发送的字节数
//                                 printf("Successfully sent %ld bytes to the client\n", sent_len);
//                             }
//                             // sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
//                         }
//                     }
//                 }
//             }

//             mysql_free_result(res);
//         }

//         pthread_mutex_unlock(&flight_mutex);
//         // printf("check in flight seat availability\n");
//         sleep(5); // 每5秒查询一次数据库
//     }

//     close_db(conn); // 关闭数据库连接
//     return NULL;
// }

void* monitor_flights(void* arg)
{
    struct client_data *data = (struct client_data *)arg;
    MYSQL *conn = connect_db(); // 连接数据库

    while (1)
    {
        pthread_mutex_lock(&flight_mutex); // 加锁，保证线程安全

        // 遍历所有监控的航班，确保只查询一次数据库
        for (int i = 0; i < client_monitor_count; i++)
        {
            int flight_id = client_monitors[i].flight_id;

            // 查询当前航班的座位可用性
            char query[256];
            snprintf(query, sizeof(query), "SELECT seat_availability FROM flights WHERE flight_id = %d", flight_id);

            if (mysql_query(conn, query))
            {
                fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
                continue;
            }

            MYSQL_RES *res = mysql_store_result(conn);
            if (res == NULL)
            {
                fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
                continue;
            }

            MYSQL_ROW row = mysql_fetch_row(res);
            if (row)
            {
                int current_seat_availability = atoi(row[0]);

                // 比较当前座位可用数量是否变化，只有变化时通知客户端
                if (client_monitors[i].seat_availability != current_seat_availability)
                {
                    client_monitors[i].seat_availability = current_seat_availability; // 更新座位信息
                    printf("Seats availability changed for flight %d, notifying clients\n", flight_id);

                    // 发送通知给所有监控该航班的客户端
                    for (int j = 0; j < client_monitor_count; j++)
                    {
                        if (client_monitors[j].flight_id == flight_id)
                        {
                            // 通知客户端
                            char response[BUFFER_SIZE];
                            snprintf(response, sizeof(response),
                                     "Flight %d seat availability updated to %d\n",
                                     flight_id, current_seat_availability);

                            // 发送消息给客户端
                            ssize_t sent_len = sendto(data->sockfd, response, strlen(response), 0,
                                                      (struct sockaddr *)&client_monitors[j].client_addr,
                                                      sizeof(client_monitors[j].client_addr));

                            // 判断发送是否成功
                            if (sent_len == -1)
                            {
                                perror("Failed to send data with sendto");
                            }
                            else
                            {
                                printf("Successfully sent %ld bytes to the client\n", sent_len);
                            }
                        }
                    }
                }
            }
            mysql_free_result(res); // 释放结果集
        }

        pthread_mutex_unlock(&flight_mutex); // 解锁

        sleep(5); // 每5秒检查一次
    }

    close_db(conn); // 关闭数据库连接
    return NULL;
}

