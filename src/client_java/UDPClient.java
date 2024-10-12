package SC6103_DS.src.client_java;




import java.net.*;
import java.io.*;
import java.util.*;
import SC6103_DS.src.client_java.*;


public class UDPClient {
    private static final String server_address = "";
    private static final int port_number = "";
    private static Callback callback;

    public static void main(String[] args) {
        // DatagramSocket aSocket = null;
        try {
        //     aSocket = new DatagramSocket();
        //     byte[] message = message.getBytes();
        //     while (true) { // shuangyue
        //         DatagramPacket request = new DatagramPacket(message, message.length);
        //         aSocket.receive(request);
        //         DatagramPacket reply =new DatagramPacket(
        //             request.getData(), request.getLength(),
        //             request.getAddress(), request.getPort()
        //         );
        //         aSocket.send(reply);
        //     }  catch (Exception e) {
        //         e.printStackTrace();
        //     }


            //InetAddress serverAddress = InetAddress.getByName(server_address);
            //String server_address = server_address;
            InetAddress serverInetAddress = InetAddress.getByName(server_address);
            callback = new Callback(serverInetAddress, port_number); // 初始化回调处理器
            runClient(serverInetAddress, port_number);
        } catch (Exception e) {
            e.printStackTrace(); // 打印异常堆栈信息
            System.out.println("Error initializing UDPClient"); // 输出错误信息
   

        }
        // if (aSocket != null)
        //     aSocket.close();

    }

     // 运行客户端逻辑
    public static void runClient(InetAddress serverInetAddress, int port_number) {
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
                        udpUtils.sendMessage(queryMessage, serverInetAddress, port_number); // 发送查询消息
                        String response = udpUtils.receiveMessage(); // 接收响应消息
                        System.out.println("Server response: " + response);
                        break;
                    case 2: // 监控座位可用性
                        System.out.print("Enter flight ID to monitor: ");
                        int flightId = Integer.parseInt(scanner.nextLine());
                        System.out.print("Enter monitor interval in seconds: ");
                        int monitorInterval = Integer.parseInt(scanner.nextLine());
                        callback.startMonitoringSeatAvailability(flightId); // 开始监控
                        break;
                    case 3: // 选择餐饮（幂等操作）
                        System.out.print("Enter flight ID to select meal for: ");
                        int mealFlightId = Integer.parseInt(scanner.nextLine());
                        System.out.print("Enter meal option: ");
                        String mealOption = scanner.nextLine();
                        String mealMessage = "SELECT_MEAL " + mealFlightId + " " + mealOption;
                        udpUtils.sendMessage(mealMessage, serverInetAddress, port_number); // 发送餐饮选择请求
                        String mealResponse = udpUtils.receiveMessage();
                        System.out.println("Server response: " + mealResponse);
                        break;
                    case 4: // baggage weight（非幂等操作）
                        System.out.print("Enter flight ID to purchase baggage survey service for: ");
                        int baggageFlightId = Integer.parseInt(scanner.nextLine());
                        String baggageMessage = "baggage_weight " + baggageFlightId;
                        udpUtils.sendMessage(baggageMessage, serverInetAddress, port_number); // 发送 VIP 购买请求
                        String baggageResponse = udpUtils.receiveMessage();
                        System.out.println("Server response: " + baggageResponse);
                        break;
                    case 9: // 退出客户端
                        System.out.println("Exiting client...");
                        callback.stopMonitoring(); // 停止所有监控
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
        System.out.println("2. Monitor seat availability");
        System.out.println("3. Select meal (Idempotent)");
        System.out.println("4. Purchase VIP service (Non-idempotent)");
        System.out.println("9. Exit");
        System.out.print("Enter your choice: ");
    }

    
        
}