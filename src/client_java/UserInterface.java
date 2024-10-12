package SC6103_DS.src.client_java;

import java.util.Scanner;

import SC6103_DS.src.communication.Message;
import SC6103_DS.src.communication.Marshalling;
import SC6103_DS.src.communication.Unmarshalling;

public class UserInterface {
    private Scanner scanner;

    public UserInterface() {
        this.scanner = new Scanner(System.in);
    }

    // Display menu options
    public void displayMenu() {
        System.out.println("Welcome to the Flight Booking System");
        System.out.println("Please select an option:");
        System.out.println("1. Query Flight by Source and Destination");
        System.out.println("2. Query Flight Information");
        System.out.println("3. Monitor Seat Availability");
        System.out.println("4. Select meal");
        System.out.println("5. Baggage weight");
        System.out.println("9. Exit");
        System.out.print("Enter your choice: ");
    }

    // Get user's choice
    public int getUserChoice() {
        int choice = -1;
        try {
            choice = Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
            System.out.println("Invalid input. Please enter a number between 1 and 9.");
        }
        return choice;
    }

    // Get flight information
    public String[] getFlightInfo() {
        System.out.print("Enter source place: ");
        String source = scanner.nextLine();
        System.out.print("Enter destination place: ");
        String destination = scanner.nextLine();
        return new String[] { source, destination };
    }

    // Get flight ID
    public int getFlightId() {
        System.out.print("Enter flight ID: ");
        return Integer.parseInt(scanner.nextLine());
    }

    // Get seat number
    public int getSeatNumber() {
        System.out.print("Enter number of seats to reserve: ");
        return Integer.parseInt(scanner.nextLine());
    }

    // Display server response
    public void displayResponse(String response) {
        System.out.println("Server response: " + response);
    }

    // Close resources
    public void close() {
        scanner.close();
    }
}
