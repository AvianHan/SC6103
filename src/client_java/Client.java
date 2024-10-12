package SC6103_DS.src.client_java;

import java.net.*;
import java.io.*;
import java.util.*;
import SC6103_DS.src.client_java.*;
import SC6103_DS.src.communication.*;

public class Client {

    private static final String server_addr = "192.168.200.1";
    private static final int server_port = 8080;
    private static Callback callback;

    public static void main(String[] args) {
        try {
            InetAddress serverInetAddress = InetAddress.getByName(server_addr);
            callback = new Callback(serverInetAddress, server_port); // Initialize callback handler
            runClient(serverInetAddress, server_port);
        } catch (Exception e) {
            e.printStackTrace(); // Print exception stack trace
            System.out.println("Error initializing UDPClient"); // Output error message
        }
    }

    // Run client logic
    public static void runClient(InetAddress serverInetAddress, int server_port) {
        try (Scanner scanner = new Scanner(System.in)) {
            UDPUtils udpUtils = new UDPUtils(); // Initialize UDP utility class

            while (true) {
                displayMenu(); // Display user menu
                int choice = Integer.parseInt(scanner.nextLine());

                switch (choice) {
                    case 1: // Query flight by source and destination
                        System.out.print("Enter source place: ");
                        String source = scanner.nextLine();
                        System.out.print("Enter destination place: ");
                        String destination = scanner.nextLine();
                        String queryMessage = "QUERY_FLIGHT " + source + " " + destination;
                        udpUtils.sendMessage(queryMessage, serverInetAddress, server_port); // Send query message
                        String response = udpUtils.receiveMessage(); // Receive response message
                        System.out.println("Server response: " + response);
                        break;
                    case 2: // Monitor flight information
                        System.out.print("Enter flight ID to monitor: ");
                        int flight_id = Integer.parseInt(scanner.nextLine());
                        String queryInfo = "query_flight_info " + flight_id;
                        udpUtils.sendMessage(queryInfo, serverInetAddress, server_port);
                        break;
                    case 3: // Monitor seat availability
                        System.out.print("Enter flight ID to monitor: ");
                        int flightId = Integer.parseInt(scanner.nextLine());
                        System.out.print("Enter monitor interval in seconds: ");
                        int monitorInterval = Integer.parseInt(scanner.nextLine());
                        callback.startMonitoringSeatAvailability(flightId, monitorInterval); // Start monitoring
                        break;
                    case 4: // Select meal (Idempotent operation)
                        System.out.print("Enter flight ID to select meal for: ");
                        int mealFlightId = Integer.parseInt(scanner.nextLine());
                        System.out.print("Enter meal option: ");
                        String mealOption = scanner.nextLine();
                        String mealMessage = "SELECT_MEAL " + mealFlightId + " " + mealOption;
                        udpUtils.sendMessage(mealMessage, serverInetAddress, server_port); // Send meal selection request
                        String mealResponse = udpUtils.receiveMessage();
                        System.out.println("Server response: " + mealResponse);
                        break;
                    case 5: // Baggage weight (Non-idempotent operation)
                        System.out.print("Enter flight ID to purchase baggage survey service for: ");
                        int baggageFlightId = Integer.parseInt(scanner.nextLine());
                        String baggageMessage = "baggage_weight " + baggageFlightId;
                        udpUtils.sendMessage(baggageMessage, serverInetAddress, server_port); // Send baggage request
                        String baggageResponse = udpUtils.receiveMessage();
                        System.out.println("Server response: " + baggageResponse);
                        break;
                    case 9: // Exit client
                        System.out.println("Exiting client...");
                        callback.stopMonitoring(); // Stop all monitoring
                        return;
                    default:
                        System.out.println("Invalid choice. Please try again.");
                        break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace(); // Print exception stack trace
            System.out.println("Error running client"); // Output error message
        }
    }

    // Display user menu
    private static void displayMenu() {
        System.out.println("\nWelcome to the Flight Information System");
        System.out.println("1. Query flight by source and destination");
        System.out.println("2. Monitor flight information");
        System.out.println("3. Monitor seat availability");
        System.out.println("4. Select meal (Idempotent)");
        System.out.println("5. Baggage weight (Non-idempotent)");
        System.out.println("9. Exit");
        System.out.print("Enter your choice: ");
    }
}
