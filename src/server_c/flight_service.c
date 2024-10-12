// 航班相关服务实现
// flight service
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#define BUFFER_SIZE 1024

extern Flight *flights;
extern int flight_count;

// 处理航班查询请求（通过出发地和目的地）
void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    char source[50], destination[50];
    sscanf(buffer, "QUERY_FLIGHT %s %s", source, destination);

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }
    response[0] = '\0';

    for (int i = 0; i < flight_count; i++) {
        if (strcmp(flights[i].source_place, source) == 0 && strcmp(flights[i].destination_place, destination) == 0) {
            char flight_info[100];
            sprintf(flight_info, "Flight ID: %d\n", flights[i].flight_id);
            strcat(response, flight_info);
        }
    }

    if (strlen(response) == 0) {
        strcpy(response, "No flights found for the given source and destination");
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    free(response);
}

// 处理航班详细信息查询请求（通过航班ID）
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id;
    sscanf(buffer, "QUERY_DETAILS %d", &flight_id);

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }

    int found = 0;
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            sprintf(response, "Flight ID: %d\nSource: %s\nDestination: %s\nDeparture Time: %d-%02d-%02d %02d:%02d\nAirfare: %.2f\nAvailable Seats: %d\n",
                    flights[i].flight_id, flights[i].source_place, flights[i].destination_place,
                    flights[i].departure_time.year, flights[i].departure_time.month, flights[i].departure_time.day,
                    flights[i].departure_time.hour, flights[i].departure_time.minute,
                    flights[i].airfare, flights[i].seat_availability);
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Flight not found");
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    free(response);
}

// 处理航班座位预订请求
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int flight_id, seats;
    sscanf(buffer, "RESERVE %d %d", &flight_id, &seats);

    char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (response == NULL) {
        perror("Memory allocation failed");
        return;
    }

    int found = 0;
    for (int i = 0; i < flight_count; i++) {
        if (flights[i].flight_id == flight_id) {
            if (flights[i].seat_availability >= seats) {
                flights[i].seat_availability -= seats;
                sprintf(response, "Reservation successful. Remaining seats: %d\n", flights[i].seat_availability);
            } else {
                strcpy(response, "Not enough available seats");
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Flight not found");
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    free(response);
}