// 数据存储头文件
// data storage
#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

// 航班结构体
typedef struct {
    int flight_id;
    char source[50];
    char destination[50];
    char departure_time[20];
    float airfare;
    int available_seats;
} Flight;

// 初始化航班信息
void initialize_flights();

// 根据航班ID查找航班
Flight* find_flight_by_id(int flight_id);

// 更新航班的座位信息
int update_flight_seats(int flight_id, int seats);

// 添加新航班
int add_flight(int flight_id, const char *source, const char *destination, const char *departure_time, float airfare, int available_seats);

#endif // DATA_STORAGE_H