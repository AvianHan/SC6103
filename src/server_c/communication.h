#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include "server.h"

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")  // 链接 Winsock 库
#else
    #include <arpa/inet.h>
#endif

// Message type constants
#define REGISTER_REQUEST 0x00
#define QUERY_FLIGHT_ID_REQUEST 0x01
#define QUERY_FLIGHT_INFO_REQUEST 0x02
#define MAKE_SEAT_RESERVATION_REQUEST 0x03
#define QUERY_BAGGAGE_AVAILABILITY_REQUEST 0x04
#define ADD_BAGGAGE_REQUEST 0x05

// Message structure
typedef struct {
    uint8_t message_type;
    uint32_t request_id;
    uint32_t data_length;
    uint8_t* data;
} Message;

// Function declarations
DepartureTime* create_departure_time(int year, int month, int day, int hour, int minute);
void free_departure_time(DepartureTime* departure_time);
Flight* create_flight(int flight_id, const char* source, const char* destination,
                      DepartureTime* departure_time, float airfare,
                      int seat_availability, int baggage_availability);
void free_flight(Flight* flight);
Message* create_message(uint8_t message_type, uint32_t request_id, Flight* flight_data);
void free_message(Message* message);
uint8_t* marshal_message(const Message* message, uint32_t* out_length);
Message* unmarshal_message(const uint8_t* byte_array);
uint8_t* marshal_flight(const Flight* flight, uint32_t* out_length);
Flight* unmarshal_flight(const uint8_t* byte_array, uint32_t* offset);

#endif // COMMUNICATION_H
