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
        printf("mysql_real_connect() failed\n");
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

// 关闭数据库连接的函数
void close_db(MYSQL *conn) {
    mysql_close(conn);
}

int main() {
    // 连接到数据库
    MYSQL *conn = connect_db();

    // 查询并显示航班数据
    query_flights(conn);

    // 关闭数据库连接
    close_db(conn);

    return 0;
}
