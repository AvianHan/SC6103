#include "communication.h"
#include <stdlib.h>
#include <string.h>

// Marshal the entire message into a byte array
Message* marshal_message(uint8_t message_type, uint32_t request_id, uint32_t data_length, uint8_t* data) {
    Message* message = malloc(sizeof(Message));
    message->message_type = message_type;
    message->request_id = htonl(request_id);
    message->data_length = htonl(data_length);

    message->data = malloc(data_length);
    memcpy(message->data, data, data_length);

    return message;
}

// Unmarshal a byte array into a Message struct
void unmarshal_message(uint8_t* byte_data, Message* message) {
    message->message_type = byte_data[0];
    message->request_id = ntohl(*(uint32_t*)(byte_data + 1));
    message->data_length = ntohl(*(uint32_t*)(byte_data + 5));

    message->data = malloc(message->data_length);
    memcpy(message->data, byte_data + 9, message->data_length);
}
