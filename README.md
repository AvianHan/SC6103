# SC6103_DS

## A Distributed Flight Information System
 - CS Architecture
 - UDP sockets
![CS Communication Flow](cs-communication-flow.png)

### Server

#### store the information of all flights
'''
flight:
{
    flight_id: int
    source_place: variable-length str
    destination_place: variable-length str
    departure_time: {
        year
        month
        day
        hour
        minute
    }
    airfare: float
    seat_availability: int // num of seats available
}
'''

#### implement services on the flights for remote access by clients
1. query_flight_id (source_place, destination_place) {
    if multiple flights match:
        return a list of all
    if no flight matches:
        return an error message
}

2. query_departure_time (flight_id)
    query_airfare (flight_id)
    query_seat_availability (flight_id)
    if flight_id does not exist:
        return an error message

3. make_seat_reservation (flight_id, num_seats) {
    if successful reservation:
        return acknowledgement to client
        update seat_availability on server 
    if incorrect user input (flight_id does not exist or insufficient available for num_seats):
        return an error message
}

4. callback (server & client): monitor_seat_availability (main pre content)

5. two more operations on the flights through client-server communication:
    - one idempotent
    - one non-idempotent

'''

### Client
1. provide an interface for users to invoke services
