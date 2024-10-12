import java.net.*;
import java.io.*;

public class UDPClient {
    private static final server_address = "";
    private static final port_number = "";

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
}