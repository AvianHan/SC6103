package SC6103_DS.client_java;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class CallbackHandler {
    private DatagramSocket socket;
    private InetAddress serverAddress;
    private int serverPort;
    private ExecutorService executorService;

    // 构造函数，初始化套接字、服务器地址和端口
    public CallbackHandler(InetAddress serverAddress, int serverPort) {
        try {
            this.socket = new DatagramSocket();
            this.serverAddress = serverAddress;
            this.serverPort = serverPort;
            this.executorService = Executors.newSingleThreadExecutor(); // 用于处理回调的单线程执行器
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error initializing CallbackHandler");
        }
    }

    // 发送请求以开始监控座位可用性
    public void startMonitoringSeatAvailability(int flightId) {
        String message = "MONITOR_SEAT_AVAILABILITY " + flightId;
        try {
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, serverAddress, serverPort);
            socket.send(packet);

            // 开启一个线程来监听服务器的回调消息
            executorService.submit(this::receiveCallback);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error sending monitoring request");
        }
    }

    // 接收服务器的回调消息
    private void receiveCallback() {
        try {
            while (true) {
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String response = new String(packet.getData(), 0, packet.getLength());
                System.out.println("Callback received: " + response);

                // 可以根据需要对回调进行处理，例如更新用户界面或通知用户
                handleCallback(response);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error receiving callback message");
        }
    }

    // 处理回调消息的逻辑
    private void handleCallback(String message) {
        // 此处可以实现对回调消息的具体处理逻辑，例如显示通知或更新数据
        System.out.println("Handling callback message: " + message);
    }

    // 停止监控和关闭资源
    public void stopMonitoring() {
        if (socket != null && !socket.isClosed()) {
            socket.close();
        }
        executorService.shutdown();
    }
}
