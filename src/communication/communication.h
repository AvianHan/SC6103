#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include "message_format.h"
#include "marshalling_utils.h"

// Top-level communication functions and structure definitions

// Message struct is defined in message_format.h
// typedef struct {
//     uint8_t message_type;       // 1-byte message type
//     uint32_t request_id;        // 4-byte unique request identifier
//     uint32_t data_length;       // 4-byte data length field
//     uint8_t* data;              // Variable-length data
// } Message;

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

#endif // COMMUNICATION_H
