package SC6103_DS.src.client_java;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import SC6103_DS.src.communication.Message;
import SC6103_DS.src.communication.Marshalling;
import SC6103_DS.src.communication.Unmarshalling;

public class Callback {
    private DatagramSocket socket; // Create a socket for communication
    private InetAddress serverAddress; // Server address
    private int serverPort; // Server port
    private ExecutorService executorService; // Create a thread pool for handling callback messages

    // Constructor to initialize the Callback object, accepting server address and port as parameters
    public Callback(InetAddress serverAddress, int serverPort) {
        try {
            // Create a new DatagramSocket for UDP communication with the server
            this.socket = new DatagramSocket();
            this.serverAddress = serverAddress;
            this.serverPort = serverPort;
            this.executorService = Executors.newSingleThreadExecutor(); // Single-thread executor for handling callbacks
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error initializing Callback");
        }
    }

    // Send a request to start monitoring seat availability
    public void startMonitoringSeatAvailability(int flightId, int monitorInterval) {
        String message = "MONITOR_SEAT_AVAILABILITY " + flightId + " " + monitorInterval;
        try {
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, serverAddress, serverPort);
            socket.send(packet);
    
            // Start a thread to receive callback messages
            executorService.submit(() -> receiveCallback(monitorInterval));
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error sending monitoring request");
        }
    }
    
    // Receive callback messages from the server
    private void receiveCallback(int monitorInterval) {
        long startTime = System.currentTimeMillis();
        long monitorIntervalMillis = monitorInterval * 1000L; // Convert seconds to milliseconds
        try {
            while (System.currentTimeMillis() - startTime < monitorIntervalMillis) {
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String response = new String(packet.getData(), 0, packet.getLength());
                System.out.println("Callback received: " + response);
    
                // Handle the callback message, e.g., update the user interface
                handleCallback(response);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error receiving callback message");
        } finally {
            stopMonitoring();
        }
    }
    
    // Logic for handling callback messages
    private void handleCallback(String message) {
        // Implement specific logic for handling callback messages here, e.g., display notification or update data
        System.out.println("Handling callback message: " + message);
    }

    // Stop monitoring and release resources
    public void stopMonitoring() {
        if (socket != null && !socket.isClosed()) {
            socket.close();
        }
        executorService.shutdown();
    }
}
