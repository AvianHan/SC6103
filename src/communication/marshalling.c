#include "communication.h"
#include <stdlib.h>
#include <string.h>

// Marshal an integer to network byte order
uint32_t marshal_int(uint32_t value) {
    return htonl(value); 
}

// Unmarshal an integer from network byte order
uint32_t unmarshal_int(uint32_t value) {
    return ntohl(value);
}

// Marshal a float to network byte order
float marshal_float(float value) {
    uint32_t* int_ptr = (uint32_t*)&value;
    uint32_t int_value = htonl(*int_ptr);
    return *(float*)&int_value;
}

// Unmarshal a float from network byte order
float unmarshal_float(float value) {
    uint32_t* int_ptr = (uint32_t*)&value;
    uint32_t int_value = ntohl(*int_ptr);
    return *(float*)&int_value;
}

// Marshal a string with a 4-byte length prefix
uint8_t* marshal_string(const char* str, uint32_t* out_length) {
    uint32_t str_length = strlen(str);
    *out_length = 4 + str_length;

    uint8_t* result = malloc(*out_length);
    uint32_t net_length = htonl(str_length);
    memcpy(result, &net_length, 4); 
    memcpy(result + 4, str, str_length);

    return result;
}

// Unmarshal a string with a 4-byte length prefix
char* unmarshal_string(const uint8_t* data, uint32_t* length) {
    *length = ntohl(*(uint32_t*)data); 
    char* str = malloc(*length + 1);   
    memcpy(str, data + 4, *length);
    str[*length] = '\0';              

    return str;
}

// Marshal a Flight structure into a byte array
uint8_t* marshal_flight(const Flight* flight, uint32_t* out_length) {
    uint32_t source_len, dest_len;
    uint8_t* source = marshal_string(flight->source_place, &source_len);
    uint8_t* dest = marshal_string(flight->destination_place, &dest_len);

    *out_length = 4 + source_len + dest_len + 5 * 4 + 4 + 4 + 4;

    uint8_t* result = malloc(*out_length);
    uint32_t offset = 0;

    memcpy(result + offset, &htonl(flight->flight_id), 4);
    offset += 4;
    memcpy(result + offset, source, source_len);
    offset += source_len;
    memcpy(result + offset, dest, dest_len);
    offset += dest_len;

    memcpy(result + offset, &htonl(flight->year), 4);
    offset += 4;
    memcpy(result + offset, &htonl(flight->month), 4);
    offset += 4;
    memcpy(result + offset, &htonl(flight->day), 4);
    offset += 4;
    memcpy(result + offset, &htonl(flight->hour), 4);
    offset += 4;
    memcpy(result + offset, &htonl(flight->minute), 4);
    offset += 4;

    uint32_t float_as_int = htonl(*(uint32_t*)&flight->airfare);
    memcpy(result + offset, &float_as_int, 4);
    offset += 4;

    memcpy(result + offset, &htonl(flight->seat_availability), 4);
    offset += 4;
    memcpy(result + offset, &htonl(flight->baggage_availability), 4);

    free(source);
    free(dest);

    return result;
}
