package SC6103_DS.client_java;

import java.net.*;
import java.io.*;
import java.net.InetAddress;

public class UDPClient {
    private static final String server_addr = "";
    private static final int server_port = "";

    public static void main(String server_address, String port_number) {
        DatagramSocket aSocket = null;
        try {
            aSocket = new DatagramSocket();
            byte[] message = message.getBytes();
            while (true) {
                DatagramPacket request = new DatagramPacket(message, message.length);
                aSocket.receive(request);
                DatagramPacket reply =new DatagramPacket(
                    request.getData(), request.getLength(),
                    request.getAddress(), request.getPort()
                );
                aSocket.send(reply);
            }
        }
        if (aSocket != null)
            aSocket.close();
    }

     // 客户端运行逻辑
     public void runClient() {
        try {
            UserInterface ui = new UserInterface();
            UDPUtils udpUtils = new UDPUtils();

            InetAddress serverAddress = InetAddress.getByName(server_address);

            while (true) {
                ui.displayMenu();
                int choice = ui.getUserChoice();

                switch (choice) {
                    case 1:
                        String[] flightInfo = ui.getFlightInfo();
                        String queryMessage = "QUERY_FLIGHT " + flightInfo[0] + " " + flightInfo[1];
                        udpUtils.sendMessage(queryMessage, serverAddress, port_number);
                        String response = udpUtils.receiveMessage();
                        ui.displayResponse(response);
                        break;
                    case 9:
                        System.out.println("Exiting client...");
                        ui.close();
                        return;
                    // 处理其他选项...
                    default:
                        System.out.println("Invalid choice. Please try again.");
                        break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}