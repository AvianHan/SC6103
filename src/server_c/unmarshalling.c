#include "communication.h"
#include <stdlib.h>
#include <string.h>

// Unmarshal an integer from a byte array
int unmarshal_int(const uint8_t* byte_array, uint32_t* offset) {
    uint32_t network_value;
    memcpy(&network_value, byte_array + *offset, 4);
    *offset += 4;
    return ntohl(network_value);
}

// Unmarshal a float from a byte array
float unmarshal_float(const uint8_t* byte_array, uint32_t* offset) {
    uint32_t network_value;
    memcpy(&network_value, byte_array + *offset, 4);
    *offset += 4;
    network_value = ntohl(network_value);
    return *(float*)&network_value;
}

// Unmarshal a string from a byte array
char* unmarshal_string(const uint8_t* byte_array, uint32_t* offset) {
    uint32_t str_len = unmarshal_int(byte_array, offset);
    char* str = malloc(str_len + 1);
    memcpy(str, byte_array + *offset, str_len);
    str[str_len] = '\0';
    *offset += str_len;
    return str;
}

// Unmarshal a DepartureTime structure from a byte array
DepartureTime* unmarshal_departure_time(const uint8_t* byte_array, uint32_t* offset) {
    DepartureTime* time = malloc(sizeof(DepartureTime));
    time->year = unmarshal_int(byte_array, offset);
    time->month = unmarshal_int(byte_array, offset);
    time->day = unmarshal_int(byte_array, offset);
    time->hour = unmarshal_int(byte_array, offset);
    time->minute = unmarshal_int(byte_array, offset);
    return time;
}

// Unmarshal a Flight structure from a byte array
Flight* unmarshal_flight(const uint8_t* byte_array, uint32_t* offset) {
    Flight* flight = malloc(sizeof(Flight));
    flight->flight_id = unmarshal_int(byte_array, offset);
    flight->source_place = unmarshal_string(byte_array, offset);
    flight->destination_place = unmarshal_string(byte_array, offset);
    flight->departure_time = unmarshal_departure_time(byte_array, offset);
    flight->airfare = unmarshal_float(byte_array, offset);
    flight->seat_availability = unmarshal_int(byte_array, offset);
    flight->baggage_availability = unmarshal_int(byte_array, offset);
}