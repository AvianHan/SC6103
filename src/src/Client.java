package src;

import java.io.IOException;
import java.net.*;

public class Client {

    private DatagramSocket socket;  // UDP socket for communication between client and server
    private InetAddress serverAddress;  // Server address
    private int serverPort = 8080;  // Server port
    private UserInterface userInterface;  // Reference to the UserInterface for displaying server responses

    // Constructor to initialize the connection to the server
    public Client() {
        try {
            socket = new DatagramSocket();  // Create a new UDP socket
            serverAddress = InetAddress.getByName("172.20.10.10");  // Set the server IP address
            startListeningToServer();  // Start the server listening thread to continuously listen for messages from the server
        } catch (SocketException | UnknownHostException e) {
            e.printStackTrace();  // Handle potential socket or host exceptions
        }
    }

    // Set the reference to the UserInterface, used to display server responses via callback
    public void setUserInterface(UserInterface userInterface) {
        this.userInterface = userInterface;
    }

    // Test the connection to the server
    public void testConnection() {
        String request = "test_connection";  // Simple connection test request
        System.out.println("testing connection to server");
        sendRequest(request);  // Send test request to the server
        System.out.println("already sent the request to server");
    }

    // Query flight ID by sending a request based on the source and destination places
    public void queryFlightId(String sourcePlace, String destinationPlace) {
        String request = "query_flight_id " + sourcePlace + " " + destinationPlace;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Query flight information by sending a request based on the flight ID
    public void queryFlightInfo(int flightId) {
        String request = "query_flight_info " + flightId;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Make seat reservation by sending a request based on the flight ID and number of seats
    public void makeSeatReservation(int flightId, int numSeats) {
        String request = "make_seat_reservation " + flightId + " " + numSeats;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Query baggage availability by sending a request based on the flight ID
    public void queryBaggageAvailability(int flightId) {
        String request = "query_baggage_availability " + flightId;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Add baggage by sending a request based on the flight ID and number of baggages
    public void addBaggage(int flightId, int numBaggages) {
        String request = "add_baggage " + flightId + " " + numBaggages;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Follow flight by sending a request based on the flight ID to continuously get flight updates
    public void followFlightId(int flightId) {
        String request = "follow_flight_id " + flightId;  // Construct the request
        sendRequest(request);  // Send the request to the server
    }

    // Return the server address
    public String getServerAddress() {
        System.out.println("getting server address");
        if (serverAddress != null) {
            System.out.println("Server address: " + serverAddress.getHostAddress());  // Output the server's IP address
            return serverAddress.getHostAddress();  // Return the server's IP address
        } else {
            System.out.println("Server address not found!");  // Output an error message if the server address is null
            return "Server address not found!";  // Return an error message
        }
    }

    // Send request to the server
    private void sendRequest(String request) {
        try {
            byte[] requestData = request.getBytes();  // Convert the request string into a byte array
            DatagramPacket packet = new DatagramPacket(requestData, requestData.length, serverAddress, serverPort);  // Construct the UDP packet
            System.out.println("Sending request: " + request);  // Output the request being sent
            socket.send(packet);  // Send the packet to the server via the UDP socket
            System.out.println("sendRequest done");  // Output that the request has been sent
        } catch (IOException e) {
            if (userInterface != null) {
                userInterface.displayResponse("Error sending request: " + e.getMessage());  // If sending fails, display an error message
            }
            System.out.println("Error sending request: " + e.getMessage());  // Output the error message
        }
    }

    // Start the thread that listens for messages from the server
    public void startListeningToServer() {
        new Thread(new ResponseListener()).start();  // Create and start a thread to listen for server responses
    }

    // Listen for server responses
    private class ResponseListener implements Runnable {
        @Override
        public void run() {
            try {
                socket.setSoTimeout(60000);  // Set the UDP socket timeout to 60 seconds
                while (true) {  // Enter an infinite loop to continuously listen for server responses
                    try {
                        byte[] buffer = new byte[1024];  // Create a buffer to store the server response data
                        DatagramPacket responsePacket = new DatagramPacket(buffer, buffer.length);  // Create a packet to receive the server response
                        System.out.println("Waiting for server response...");
                        socket.receive(responsePacket);  // Receive the response from the server via the UDP socket

                        // Convert the received byte data into a string
                        String response = new String(responsePacket.getData(), 0, responsePacket.getLength());
                        System.out.println("Received response from server: " + response);  // Output the received response

                        // If the user interface exists, display the response on the interface
                        if (userInterface != null) {
                            userInterface.displayResponse("Response: " + response);
                        }

                    } catch (SocketTimeoutException e) {
                        // Handle timeout, and continue waiting for a new response
                        System.out.println("Request timed out: No response from server within the timeout period.");
                        if (userInterface != null) {
                            userInterface.displayResponse("Request timed out: No response from server.");
                        }
                    }
                }
            } catch (IOException e) {
                // If an IO exception occurs, display the error message
                if (userInterface != null) {
                    userInterface.displayResponse("Error receiving response: " + e.getMessage());
                }
                System.out.println("Error receiving response: " + e.getMessage());
            }
        }
    }
}
