package SC6103_DS.src.client_java;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Callback {
    private DatagramSocket socket; // 创建一个套接字，用于通信
    private InetAddress serverAddress; // 服务器地址
    private int serverPort; // 服务器端口
    private ExecutorService executorService; // 创建一个线程池，用于处理回调消息。

    // 构造函数，初始化 Callback 对象，接收服务器地址和端口作为参数
    public Callback(InetAddress serverAddress, int serverPort) {
        try {
            // 创建一个新的 DatagramSocket，用于与服务器进行 UDP 通信
            this.socket = new DatagramSocket();
            this.serverAddress = serverAddress;
            this.serverPort = serverPort;
            this.executorService = Executors.newSingleThreadExecutor(); // 用于处理回调的单线程执行器
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error initializing Callback");
        }
    }

    // 发送请求以开始监控座位可用性
    public void startMonitoringSeatAvailability(int flightId, int monitorInterval) {
        String message = "MONITOR_SEAT_AVAILABILITY " + flightId + " " + monitorInterval;
        try {
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, serverAddress, serverPort);
            socket.send(packet);
    
            // 启动一个线程来接收回调消息
            executorService.submit(this::receiveCallback);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error sending monitoring request");
        }
    }
    

    // 接收服务器的回调消息
    private void receiveCallback() {
        long startTime = System.currentTimeMillis();
        long monitorIntervalMillis = monitorInterval * 1000L; // 将秒数转换为毫秒
        try {
            while (System.currentTimeMillis() - startTime < monitorIntervalMillis) {
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);
                String response = new String(packet.getData(), 0, packet.getLength());
                System.out.println("Callback received: " + response);
    
                // 处理回调消息，例如更新用户界面
                handleCallback(response);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Error receiving callback message");
        } finally {
            stopMonitoring();
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
