#include "communication.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "server.h"


// Marshal an integer to a byte array (4 bytes)
uint8_t* marshal_int(int value) {
    uint32_t network_value = htonl(value);
    uint8_t* buffer = malloc(4);
    memcpy(buffer, &network_value, 4);
    return buffer;
}

// Marshal a float to a byte array (4 bytes)
uint8_t* marshal_float(float value) {
    uint32_t* int_rep = (uint32_t*)&value;
    uint32_t network_value = htonl(*int_rep);
    uint8_t* buffer = malloc(4);
    memcpy(buffer, &network_value, 4);
    return buffer;
}

// Marshal a string with length prefix
uint8_t* marshal_string(const char* str, uint32_t* out_length) {
    uint32_t str_len = strlen(str);
    *out_length = 4 + str_len;
    uint8_t* buffer = malloc(*out_length);

    uint32_t network_len = htonl(str_len);
    memcpy(buffer, &network_len, 4);
    memcpy(buffer + 4, str, str_len);
    return buffer;
}

// Marshal a DepartureTime structure
uint8_t* marshal_departure_time(const DepartureTime* departure, uint32_t* out_length) {
    *out_length = 5 * 4;
    uint8_t* buffer = malloc(*out_length);

    memcpy(buffer, marshal_int(departure->year), 4);
    memcpy(buffer + 4, marshal_int(departure->month), 4);
    memcpy(buffer + 8, marshal_int(departure->day), 4);
    memcpy(buffer + 12, marshal_int(departure->hour), 4);
    memcpy(buffer + 16, marshal_int(departure->minute), 4);

    return buffer;
}

// Marshal a Flight structure
uint8_t* marshal_flight(const Flight* flight, uint32_t* out_length) {
    uint32_t source_len, dest_len, time_len;
    uint8_t* source = marshal_string(flight->source_place, &source_len);
    uint8_t* dest = marshal_string(flight->destination_place, &dest_len);
    uint8_t* time = marshal_departure_time(&(flight->departure_time), &time_len);

    *out_length = 4 + source_len + dest_len + time_len + 4 + 4 + 4;
    uint8_t* buffer = malloc(*out_length);

    uint32_t offset = 0;
    memcpy(buffer + offset, marshal_int(flight->flight_id), 4); offset += 4;
    memcpy(buffer + offset, source, source_len); offset += source_len;
    memcpy(buffer + offset, dest, dest_len); offset += dest_len;
    memcpy(buffer + offset, time, time_len); offset += time_len;
    memcpy(buffer + offset, marshal_float(flight->airfare), 4); offset += 4;
    memcpy(buffer + offset, marshal_int(flight->seat_availability), 4); offset += 4;
    memcpy(buffer + offset, marshal_int(flight->baggage_availability), 4);

    free(source);
    free(dest);
    free(time);

    return buffer;
}

// Marshal a Message structure
uint8_t* marshal_message(const Message* message, uint32_t* out_length) {
    *out_length = 1 + 4 + 4 + message->data_length;
    uint8_t* buffer = malloc(*out_length);

    buffer[0] = message->message_type;
    memcpy(buffer + 1, marshal_int(message->request_id), 4);
    memcpy(buffer + 5, marshal_int(message->data_length), 4);
    memcpy(buffer + 9, message->data, message->data_length);

    return buffer;
}
