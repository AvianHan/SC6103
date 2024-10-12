#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

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

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    // 输出查询结果
    while ((row = mysql_fetch_row(result))) {
        for (int i = 0; i < num_fields; i++) {
            printf("%s\t", row[i] ? row[i] : "NULL");
        }
        printf("\n");
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
