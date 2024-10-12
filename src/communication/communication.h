#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <arpa/inet.h>

// Message type constants
#define REGISTER_REQUEST 0x00
#define QUERY_FLIGHT_ID_REQUEST 0x01
#define QUERY_FLIGHT_INFO_REQUEST 0x02
#define MAKE_SEAT_RESERVATION_REQUEST 0x03
#define QUERY_BAGGAGE_AVAILABILITY_REQUEST 0x04
#define ADD_BAGGAGE_REQUEST 0x05

// Message structure
typedef struct {
    uint8_t message_type;       // 1-byte message type
    uint32_t request_id;        // 4-byte unique request identifier
    uint32_t data_length;       // 4-byte data length field
    uint8_t* data;              // Variable-length data
} Message;

// Flight structure
typedef struct {
    int flight_id;
    char* source_place;
    char* destination_place;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    float airfare;
    int seat_availability;
    int baggage_availability;
} Flight;

// Functions for marshaling and unmarshaling messages
Message* marshal_message(uint8_t message_type, uint32_t request_id, uint32_t data_length, uint8_t* data);
void unmarshal_message(uint8_t* byte_data, Message* message);

// Functions for marshaling and unmarshaling integers
uint32_t marshal_int(uint32_t value);
uint32_t unmarshal_int(uint32_t value);

// Functions for marshaling and unmarshaling floats
float marshal_float(float value);
float unmarshal_float(float value);

// Functions for marshaling and unmarshaling strings
uint8_t* marshal_string(const char* str, uint32_t* out_length);
char* unmarshal_string(const uint8_t* data, uint32_t* length);

// Functions for marshaling and unmarshaling Flight structure
uint8_t* marshal_flight(const Flight* flight, uint32_t* out_length);
Flight* unmarshal_flight(const uint8_t* data);

#endif // COMMUNICATION_H
