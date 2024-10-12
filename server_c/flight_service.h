// 航班服务头文件

// flight_service.h
#ifndef FLIGHT_SERVICE_H
#define FLIGHT_SERVICE_H

#include <netinet/in.h>

void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer);

#endif
