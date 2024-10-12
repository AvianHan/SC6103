package SC6103_DS.client_java;

import java.util.Scanner;

public class UserInterface {
    private Scanner scanner;

    public UserInterface() {
        this.scanner = new Scanner(System.in);
    }

    // 显示菜单选项
    public void displayMenu() {
        System.out.println("Welcome to the Flight Booking System");
        System.out.println("Please select an option:");
        System.out.println("1. Query Flight by Source and Destination");
        System.out.println("2. Query Departure Time");
        System.out.println("3. Query Airfare");
        System.out.println("4. Query Seat Availability");
        System.out.println("5. Make Seat Reservation");
        System.out.println("6. Monitor Seat Availability");
        System.out.println("7. Select Meal (Idempotent)");
        System.out.println("8. Purchase VIP Lounge Access (Non-idempotent)");
        System.out.println("9. Exit");
        System.out.print("Enter your choice: ");
    }

    // 获取用户选择
    public int getUserChoice() {
        int choice = -1;
        try {
            choice = Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
            System.out.println("Invalid input. Please enter a number between 1 and 9.");
        }
        return choice;
    }

    // 获取航班信息
    public String[] getFlightInfo() {
        System.out.print("Enter source place: ");
        String source = scanner.nextLine();
        System.out.print("Enter destination place: ");
        String destination = scanner.nextLine();
        return new String[] { source, destination };
    }

    // 获取航班ID
    public int getFlightId() {
        System.out.print("Enter flight ID: ");
        return Integer.parseInt(scanner.nextLine());
    }

    // 获取座位数量
    public int getSeatNumber() {
        System.out.print("Enter number of seats to reserve: ");
        return Integer.parseInt(scanner.nextLine());
    }

    // 显示服务器响应
    public void displayResponse(String response) {
        System.out.println("Server response: " + response);
    }

    // 关闭资源
    public void close() {
        scanner.close();
    }
}
