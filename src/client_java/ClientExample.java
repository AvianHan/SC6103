package SC6103_DS.src.client_java;

public class ClientExample {
    public static void main(String[] args) {
        // Generate a request to query flights from "JFK" to "LAX"
        byte[] request = MessageHandler.generate_query_flight_id_request("JFK", "LAX");

        // Mock server response for testing
        byte[] mockResponse = new byte[] {0, 0, 0, 1, 0, 0, 0, 42}; // 1 flight with ID 42
        MessageHandler.handle_query_flight_id_response(mockResponse);
    }
}

