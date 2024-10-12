// 回调处理头文件
// callback_handler
#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <netinet/in.h>

// 注册客户端以监控航班座位可用性更新
void register_callback(int sockfd, struct sockaddr_in *client_addr, int flight_id, int monitor_interval);

#endif // CALLBACK_HANDLER_H