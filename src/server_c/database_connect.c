#include "server.h"    // This must include Flight struct definition
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>    // For strdup and other string functions

// 数据库连接信息
#define HOST "localhost"
#define USER "root"
#define PASS "root"
#define DB "flight_system"

// 连接到数据库的函数
MYSQL* connect_db() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("mysql_init() failed\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(conn, HOST, USER, PASS, DB, 0, NULL, 0) == NULL) {
        printf("mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    return conn;
}

// 查询航班数据的函数
void query_flights(MYSQL *conn) {
    const char *query = "SELECT flight_id, source_place, destination_place, departure_year, departure_month, departure_day, departure_hour, departure_minute, airfare, seat_availability, baggage_availability FROM flights";

    if (mysql_query(conn, query)) {
        printf("QUERY failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result() failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        Flight flight;
        flight.flight_id = atoi(row[0]);
        flight.source_place = strdup(row[1]);
        flight.destination_place = strdup(row[2]);

        // 将查询结果填充到 DepartureTime 结构体中
        flight.departure_time.year = atoi(row[3]);
        flight.departure_time.month = atoi(row[4]);
        flight.departure_time.day = atoi(row[5]);
        flight.departure_time.hour = atoi(row[6]);
        flight.departure_time.minute = atoi(row[7]);

        flight.airfare = atof(row[8]);
        flight.seat_availability = atoi(row[9]);
        flight.baggage_availability = atoi(row[10]);

        // 打印航班信息
        printf("Flight ID: %d\n", flight.flight_id);
        printf("Source: %s\n", flight.source_place);
        printf("Destination: %s\n", flight.destination_place);
        printf("Departure Time: %d-%d-%d %d:%d\n", flight.departure_time.year, flight.departure_time.month, flight.departure_time.day, flight.departure_time.hour, flight.departure_time.minute);
        printf("Airfare: %.2f\n", flight.airfare);
        printf("Seats Available: %d\n", flight.seat_availability);
        printf("Baggage Availability: %d\n\n", flight.baggage_availability);
    }

    mysql_free_result(result);
}

// 更新航班座位数的函数
void update_seats(MYSQL *conn, int flight_id, int seats_reserved) {
    char query[256];

    // 首先检查当前座位数
    sprintf(query, "SELECT seat_availability FROM flights WHERE flight_id = %d", flight_id);
    if (mysql_query(conn, query)) {
        printf("SELECT QUERY failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result() failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        int available_seats = atoi(row[0]);
        if (available_seats < seats_reserved) {
            printf("Not enough seats available.\n");
        } else {
            // 如果座位足够，更新座位数
            sprintf(query, "UPDATE flights SET seat_availability = seat_availability - %d WHERE flight_id = %d", seats_reserved, flight_id);
            if (mysql_query(conn, query)) {
                printf("UPDATE QUERY failed: %s\n", mysql_error(conn));
            } else {
                printf("Seats updated successfully.\n");
            }
        }
    } else {
        printf("Flight not found.\n");
    }

    mysql_free_result(result);
}

// 更新航班行李的函数
void update_baggage(MYSQL *conn, int flight_id, int baggage_added) {
    char query[256];

    // 检查当前行李可用量
    sprintf(query, "SELECT baggage_availability FROM flights WHERE flight_id = %d", flight_id);
    if (mysql_query(conn, query)) {
        printf("SELECT QUERY failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result() failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        int available_baggage = atoi(row[0]);
        if (available_baggage < baggage_added) {
            printf("Not enough baggage space available.\n");
        } else {
            // 更新行李空间
            sprintf(query, "UPDATE flights SET baggage_availability = baggage_availability - %d WHERE flight_id = %d", baggage_added, flight_id);
            if (mysql_query(conn, query)) {
                printf("UPDATE QUERY failed: %s\n", mysql_error(conn));
            } else {
                printf("Baggage updated successfully.\n");
            }
        }
    } else {
        printf("Flight not found.\n");
    }

    mysql_free_result(result);
}

// 关闭数据库连接的函数
void close_db(MYSQL *conn) {
    mysql_close(conn);
}

int main() {
    // 连接到数据库
    MYSQL *conn = connect_db();

    // 查询并显示航班数据
    printf("Current flights:\n");
    query_flights(conn);

    // 更新座位数（示例）
    int flight_id = 1; // 示例航班ID
    int seats_to_reserve = 2;
    printf("\nUpdating seats for flight %d\n", flight_id);
    update_seats(conn, flight_id, seats_to_reserve);

    // 更新行李（示例）
    int baggage_to_add = 10;
    printf("\nUpdating baggage for flight %d\n", flight_id);
    update_baggage(conn, flight_id, baggage_to_add);

    // 关闭数据库连接
    close_db(conn);

    return 0;
}
