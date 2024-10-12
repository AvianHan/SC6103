#include "marshalling_utils.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// Marshals an integer to network byte order
uint32_t marshal_int(uint32_t value) {
    return htonl(value); // Convert host byte order to network byte order
}

// Unmarshals an integer from network byte order
uint32_t unmarshal_int(uint32_t value) {
    return ntohl(value); // Convert network byte order to host byte order
}

// Marshals a float to network byte order
float marshal_float(float value) {
    uint32_t* int_ptr = (uint32_t*)&value;
    uint32_t int_value = htonl(*int_ptr);
    return *(float*)&int_value;
}

// Unmarshals a float from network byte order
float unmarshal_float(float value) {
    uint32_t* int_ptr = (uint32_t*)&value;
    uint32_t int_value = ntohl(*int_ptr);
    return *(float*)&int_value;
}

// Marshals a string with a 4-byte length prefix
uint8_t* marshal_string(const char* str, uint32_t* out_length) {
    uint32_t str_length = strlen(str);
    *out_length = 4 + str_length;

    uint8_t* result = malloc(*out_length);
    uint32_t net_length = htonl(str_length);
    memcpy(result, &net_length, 4); 
    memcpy(result + 4, str, str_length);

    return result;
}

// Unmarshals a string with a 4-byte length prefix
char* unmarshal_string(const uint8_t* data, uint32_t* length) {
    *length = ntohl(*(uint32_t*)data); 
    char* str = malloc(*length + 1); 
    memcpy(str, data + 4, *length);
    str[*length] = '\0'; 

    return str;
}
