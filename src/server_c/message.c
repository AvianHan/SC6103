#include "communication.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "server.h"

// Function to create a new DepartureTime object
DepartureTime* create_departure_time(int year, int month, int day, int hour, int minute) {
    DepartureTime* departure_time = (DepartureTime*)malloc(sizeof(DepartureTime));
    departure_time->year = year;
    departure_time->month = month;
    departure_time->day = day;
    departure_time->hour = hour;
    departure_time->minute = minute;
    return departure_time;
}

// Function to free a DepartureTime object
void free_departure_time(DepartureTime* departure_time) {
    if (departure_time) {
        free(departure_time);
    }
}

// Function to create a new Flight object
Flight* create_flight(int flight_id, const char* source, const char* destination,
                      DepartureTime* departure_time, float airfare,
                      int seat_availability, int baggage_availability) {
    Flight* flight = (Flight*)malloc(sizeof(Flight));
    flight->flight_id = flight_id;

    // Allocate memory and copy strings for source and destination
    flight->source_place = (char*)malloc(strlen(source) + 1);
    strcpy(flight->source_place, source);

    flight->destination_place = (char*)malloc(strlen(destination) + 1);
    strcpy(flight->destination_place, destination);

    // Copy the pointer for DepartureTime
    flight->departure_time = departure_time;

    flight->airfare = airfare;
    flight->seat_availability = seat_availability;
    flight->baggage_availability = baggage_availability;

    return flight;
}

// Function to free a Flight object, including its strings and DepartureTime
void free_flight(Flight* flight) {
    if (flight) {
        free(flight->source_place);
        free(flight->destination_place);
        free_departure_time(flight->departure_time); // Free DepartureTime
        free(flight);
    }
}

// Function to create a new Message object with Flight data
Message* create_message(uint8_t message_type, uint32_t request_id, Flight* flight_data) {
    Message* message = (Message*)malloc(sizeof(Message));
    message->message_type = message_type;
    message->request_id = request_id;

    // Marshal the Flight data into the data field of the Message
    message->data = marshal_flight(flight_data, &message->data_length);

    return message;
}

// Function to free a Message object
void free_message(Message* message) {
    if (message) {
        free(message->data);
        free(message);
    }
}
