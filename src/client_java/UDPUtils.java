package SC6103_DS.src.client_java;

import SC6103_DS.communication.*;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPUtils {
    private DatagramSocket socket;

    // 构造函数，创建一个UDP套接字
    public UDPUtils() {
        try {
            this.socket = new DatagramSocket();
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error initializing DatagramSocket");
        }
    }

    // 发送消息到指定的服务器地址和端口
    public void sendMessage(String message, InetAddress serverAddress, int serverPort) {
        try {
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, serverAddress, serverPort);
            socket.send(packet);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error sending message: " + message);
        }
    }

    // 接收服务器的响应消息
    public String receiveMessage() {
        try {
            byte[] buffer = new byte[1024]; // 定义一个缓冲区来接收数据
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
            socket.receive(packet);
            return new String(packet.getData(), 0, packet.getLength());
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error receiving message");
        }
        return null;
    }

    // 关闭UDP套接字
    public void closeSocket() {
        if (socket != null && !socket.isClosed()) {
            socket.close();
        }
    }
}
