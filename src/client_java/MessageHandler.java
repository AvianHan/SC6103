package SC6103_DS.src.client_java;

public class MessageHandler {

    // Generate a QUERY_FLIGHT_ID request message
    public static byte[] generate_query_flight_id_request(String source, String destination) {
        byte[] sourceBytes = Marshalling.encode_string(source);
        byte[] destinationBytes = Marshalling.encode_string(destination);

        // Combine source and destination bytes into one payload
        byte[] payload = new byte[sourceBytes.length + destinationBytes.length];
        System.arraycopy(sourceBytes, 0, payload, 0, sourceBytes.length);
        System.arraycopy(destinationBytes, 0, payload, sourceBytes.length, destinationBytes.length);

        // Build the full message with message_type = 0x01
        return Marshalling.build_message(0x01, generate_request_id(), payload);
    }

    // Generate a QUERY_FLIGHT_INFO request message
    public static byte[] generate_query_flight_info_request(int flight_id) {
        byte[] payload = Marshalling.encode_int(flight_id);
        // Build the full message with message_type = 0x02
        return Marshalling.build_message(0x02, generate_request_id(), payload);
    }

    // Generate a MAKE_SEAT_RESERVATION request message
    public static byte[] generate_make_seat_reservation_request(int flight_id, int num_seats) {
        byte[] flightIdBytes = Marshalling.encode_int(flight_id);
        byte[] numSeatsBytes = Marshalling.encode_int(num_seats);

        // Combine flight ID and number of seats into one payload
        byte[] payload = new byte[flightIdBytes.length + numSeatsBytes.length];
        System.arraycopy(flightIdBytes, 0, payload, 0, flightIdBytes.length);
        System.arraycopy(numSeatsBytes, 0, payload, flightIdBytes.length, numSeatsBytes.length);

        // Build the full message with message_type = 0x03
        return Marshalling.build_message(0x03, generate_request_id(), payload);
    }

    // Handle the response for QUERY_FLIGHT_ID
    public static void handle_query_flight_id_response(byte[] payload) {
        int numFlights = Unmarshalling.decode_int(payload, 0);
        System.out.println("Number of flights found: " + numFlights);
        for (int i = 0; i < numFlights; i++) {
            int flightId = Unmarshalling.decode_int(payload, 4 + i * 4);
            System.out.println("Flight ID: " + flightId);
        }
    }

    // Handle the response for QUERY_FLIGHT_INFO
    public static void handle_query_flight_info_response(byte[] payload) {
        Message.DepartureTime departureTime = Unmarshalling.decode_departure_time(payload, 0);
        float airfare = Unmarshalling.decode_float(payload, 20);
        int seatAvailable = Unmarshalling.decode_int(payload, 24);
        
        System.out.println("Departure Time: " + departureTime);
        System.out.println("Airfare: " + airfare);
        System.out.println("Seats Available: " + seatAvailable);
    }

    // Handle the response for MAKE_SEAT_RESERVATION
    public static void handle_make_seat_reservation_response(byte[] payload) {
        int success = Unmarshalling.decode_int(payload, 0);
        if (success == 1) {
            System.out.println("Seat reservation successful.");
        } else {
            System.out.println("Seat reservation failed.");
        }
    }

    // Helper function to generate unique request IDs
    private static int generate_request_id() {
        // Generate a simple random request ID for now
        return (int) (Math.random() * 10000);
    }
}

