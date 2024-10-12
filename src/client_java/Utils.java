// package SC6103_DS.src.client_java;

//import SC6103_DS.communication.*;
import SC6103_DS.src.communication.Message;
import SC6103_DS.src.communication.Marshalling;
import SC6103_DS.src.communication.Unmarshalling;


import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class Utils {
    private DatagramSocket socket;

    // 构造函数，创建一个UDP套接字
    public Utils() {
        try {
            this.socket = new DatagramSocket();
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error initializing DatagramSocket");
        }
    }

    // 发送消息到指定的服务器地址和端口
    public void sendMessage(int messageType, Message.Flight data, String addr, int port) {
        int uniqueIdentifier = 1; // need to modify later!!!!
        int requestId = (addr.hashCode() << 16) | (port << 8) | uniqueIdentifier;
        Message message = new Message(messageType, requestId, data);
        try {
            byte[] marshaledMsg = Marshalling.marshalMessage(message);
            DatagramPacket packet = new DatagramPacket(marshaledMsg, marshaledMsg.length, addr, port);
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
