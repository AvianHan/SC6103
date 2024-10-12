private static final String SERVER_IP = "127.0.0.1"; // 服务器 IP 地址
private static final int SERVER_PORT = 9876; // 服务器端口号
private static CallbackHandler callbackHandler;

public static void main(String[] args) {
    try {
        String serverAddress = SERVER_IP;
        callbackHandler = new CallbackHandler(InetAddress.getByName(serverAddress), SERVER_PORT); // 初始化回调处理器
        runClient(serverAddress, SERVER_PORT);
    } catch (Exception e) {
        e.printStackTrace(); // 打印异常堆栈信息
        System.out.println("Error initializing UDPClient"); // 输出错误信息
    }
}

// 运行客户端逻辑
public static void runClient(String serverAddress, int serverPort) {
    try (Scanner scanner = new Scanner(System.in)) {
        UDPUtils udpUtils = new UDPUtils(); // 初始化 UDP 工具类

        while (true) {
            displayMenu(); // 显示用户菜单
            int choice = Integer.parseInt(scanner.nextLine());

            switch (choice) {
                case 1: // 查询航班
                    System.out.print("Enter source place: ");
                    String source = scanner.nextLine();
                    System.out.print("Enter destination place: ");
                    String destination = scanner.nextLine();
                    String queryMessage = "QUERY_FLIGHT " + source + " " + destination;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort); // 发送查询消息
                    String response = udpUtils.receiveMessage(); // 接收响应消息
                    System.out.println("Server response: " + response);
                    break;
                case 2: // 查询航班信息（起飞时间、票价、座位可用性）
                    System.out.print("Enter flight ID to query: ");
                    int flightId = Integer.parseInt(scanner.nextLine());
                    String timeMessage = "QUERY_DEPARTURE_TIME " + flightId;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort);
                    String timeResponse = udpUtils.receiveMessage();
                    System.out.println("Departure Time: " + timeResponse);

                    String airfareMessage = "QUERY_AIRFARE " + flightId;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort);
                    String airfareResponse = udpUtils.receiveMessage();
                    System.out.println("Airfare: " + airfareResponse);

                    String availabilityMessage = "QUERY_SEAT_AVAILABILITY " + flightId;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort);
                    String availabilityResponse = udpUtils.receiveMessage();
                    System.out.println("Seat Availability: " + availabilityResponse);
                    break;
                case 3: // 预订座位
                    System.out.print("Enter flight ID to reserve seats for: ");
                    int reserveFlightId = Integer.parseInt(scanner.nextLine());
                    System.out.print("Enter number of seats to reserve: ");
                    int numSeats = Integer.parseInt(scanner.nextLine());
                    String reservationMessage = "MAKE_SEAT_RESERVATION " + reserveFlightId + " " + numSeats;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort);
                    String reservationResponse = udpUtils.receiveMessage();
                    System.out.println("Server response: " + reservationResponse);
                    break;
                case 4: // 选择餐饮（幂等操作）
                    System.out.print("Enter flight ID to select meal for: ");
                    int mealFlightId = Integer.parseInt(scanner.nextLine());
                    System.out.print("Enter meal option: ");
                    String mealOption = scanner.nextLine();
                    String mealMessage = "SELECT_MEAL " + mealFlightId + " " + mealOption;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort); // 发送餐饮选择请求
                    String mealResponse = udpUtils.receiveMessage();
                    System.out.println("Server response: " + mealResponse);
                    break;
                case 5: // 添加额外行李（非幂等操作）
                    System.out.print("Enter flight ID to add extra baggage for: ");
                    int baggageFlightId = Integer.parseInt(scanner.nextLine());
                    System.out.print("Enter baggage weight: ");
                    int baggageWeight = Integer.parseInt(scanner.nextLine());
                    String baggageMessage = "ADD_EXTRA_BAGGAGE " + baggageFlightId + " " + baggageWeight;
                    udpUtils.sendMessage(message, InetAddress.getByName(serverAddress), serverPort); // 发送行李添加请求
                    String baggageResponse = udpUtils.receiveMessage();
                    System.out.println("Server response: " + baggageResponse);
                    break;
                case 6: // 监控座位可用性
                    System.out.print("Enter flight ID to monitor: ");
                    int monitorFlightId = Integer.parseInt(scanner.nextLine());
                    System.out.print("Enter monitor interval in seconds: ");
                    int monitorInterval = Integer.parseInt(scanner.nextLine());
                    callbackHandler.startMonitoringSeatAvailability(monitorFlightId); // 开始监控
                    break;
                case 9: // 退出客户端
                    System.out.println("Exiting client...");
                    callbackHandler.stopMonitoring(); // 停止所有监控
                    return;
                default:
                    System.out.println("Invalid choice. Please try again.");
                    break;
            }
        }
    } catch (Exception e) {
        e.printStackTrace(); // 打印异常堆栈信息
        System.out.println("Error running client"); // 输出错误信息
    }
}

// 显示用户菜单
private static void displayMenu() {
    System.out.println("\nWelcome to the Flight Information System");
    System.out.println("1. Query flight by source and destination");
    System.out.println("2. Query flight information (departure time, airfare, seat availability)");
    System.out.println("3. Make seat reservation");
    System.out.println("4. Select meal (Idempotent)");
    System.out.println("5. Add extra baggage (Non-idempotent)");
    System.out.println("6. Monitor seat availability");
    System.out.println("9. Exit");
    System.out.print("Enter your choice: ");
}