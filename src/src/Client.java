package src;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Client {

    private DatagramSocket socket;
    private InetAddress serverAddress;
    private int serverPort = 8080;  // 假设服务器端口是8080
    private UserInterface userInterface;

    // 构造函数，初始化与服务器的连接
    public Client() {
        try {
            socket = new DatagramSocket();
            serverAddress = InetAddress.getByName("172.20.10.10");  // 假设服务器是本地地址
        } catch (SocketException | UnknownHostException e) {
            e.printStackTrace();
        }
    }

    // 设置 UserInterface 的引用，用于回调显示服务器的响应
    public void setUserInterface(UserInterface userInterface) {
        this.userInterface = userInterface;
    }

    // 测试与服务器的连接
    public void testConnection() {
        String request = "test_connection";  // 简单的测试连接请求
        System.out.println("testing connection to server");
        sendRequest(request);  // 发送测试请求到服务器
        System.out.println("already sent the request to server");
    }

    // 查询航班ID
    public void queryFlightId(String sourcePlace, String destinationPlace) {
        String request = "query_flight_id " + sourcePlace + " " + destinationPlace;

        sendRequest(request);  // 发送请求到服务器
    }

    // 查询航班信息
    public void queryFlightInfo(int flightId) {
        String request = "query_flight_info " + flightId;
        sendRequest(request);
    }

    // 预订座位
    public void makeSeatReservation(int flightId, int numSeats) {
        String request = "make_seat_reservation " + flightId + " " + numSeats;
        sendRequest(request);
    }

    // 查询行李信息
    public void queryBaggageAvailability(int flightId) {
        String request = "query_baggage_availability " + flightId;
        sendRequest(request);
    }

    // 添加行李
    public void addBaggage(int flightId, int numBaggages) {
        String request = "add_baggage " + flightId + " " + numBaggages;
        sendRequest(request);
    }

    // 返回服务器地址
    public String getServerAddress() {
        System.out.println("getting server address");
        if (serverAddress != null) {
            System.out.println("Server address: " + serverAddress.getHostAddress());
            return serverAddress.getHostAddress();
        } else {
            System.out.println("Server address not found!");
            return "Server address not found!";
        }
    }

//    private void sendRequest(String requestType, Object... params) {
//        try {
//            // 计算请求类型的字节数组
//            byte[] requestTypeBytes = requestType.getBytes("UTF-8");
//
//            // 计算参数的总长度
//            int totalLength = 4 + requestTypeBytes.length; // 请求类型长度 + 请求类型本身
//
//            for (Object param : params) {
//                if (param instanceof Integer) {
//                    totalLength += 4; // 整数占4字节
//                } else if (param instanceof String) {
//                    byte[] strBytes = ((String) param).getBytes("UTF-8");
//                    totalLength += 4 + strBytes.length; // 字符串长度 + 字符串本身
//                }
//            }
//
//            // 使用 ByteBuffer 封装请求
//            ByteBuffer buffer = ByteBuffer.allocate(totalLength);
//            buffer.order(ByteOrder.BIG_ENDIAN);
//
//            // 放入请求类型长度和请求类型
//            buffer.putInt(requestTypeBytes.length);
//            buffer.put(requestTypeBytes);
//
//            // 放入参数
//            for (Object param : params) {
//                if (param instanceof Integer) {
//                    buffer.putInt((Integer) param);
//                } else if (param instanceof String) {
//                    byte[] strBytes = ((String) param).getBytes("UTF-8");
//                    buffer.putInt(strBytes.length);
//                    buffer.put(strBytes);
//                }
//            }
//
//            // 发送请求
//            byte[] requestData = buffer.array();
//            DatagramPacket packet = new DatagramPacket(requestData, requestData.length, serverAddress, serverPort);
//            System.out.println("Sending request: " + requestType + " with parameters " + java.util.Arrays.toString(params));
//            socket.send(packet);
//
//
//        } catch (UnsupportedEncodingException e) {
//            e.printStackTrace();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//    }

//
//    // 监听服务器请求 unmarshalling
//    private class ResponseListener implements Runnable {
//        @Override
//        public void run() {
//            try {
//                // 设置超时时间，单位为毫秒
//                socket.setSoTimeout(60000);  // 超时时间为 60 秒
//
//                // 准备接收缓冲区
//                byte[] buffer = new byte[1024];
//                DatagramPacket responsePacket = new DatagramPacket(buffer, buffer.length);
//                System.out.println("Waiting for server response...");
//                socket.receive(responsePacket);  // 接收服务器的响应
//
//                // 使用 ByteBuffer 解封装响应
//                ByteBuffer responseBuffer = ByteBuffer.wrap(responsePacket.getData(), 0, responsePacket.getLength());
//                responseBuffer.order(ByteOrder.BIG_ENDIAN); // 设置字节序为大端（网络字节序）
//
//                // 读取请求类型的长度
//                int requestTypeLength = responseBuffer.getInt();
//                byte[] requestTypeBytes = new byte[requestTypeLength];
//                responseBuffer.get(requestTypeBytes);
//                String requestType = new String(requestTypeBytes, "UTF-8");
//                System.out.println("Received request type: " + requestType);
//
//                // 解析请求的参数
//                StringBuilder parsedResponse = new StringBuilder("Request Type: " + requestType + ", Parameters: ");
//
//                while (responseBuffer.remaining() > 0) {
//                    // 根据参数类型顺序逐个解析参数
//                    if (responseBuffer.remaining() >= 4) {
//                        int paramLength = responseBuffer.getInt(); // 读取参数的长度
//                        if (paramLength > 0 && responseBuffer.remaining() >= paramLength) {
//                            byte[] paramBytes = new byte[paramLength];
//                            responseBuffer.get(paramBytes);
//                            String param = new String(paramBytes, "UTF-8");
//                            parsedResponse.append(param).append(" ");
//                        }
//                    }
//                }
//
//                // 打印并显示解析出的内容
//                System.out.println("Parsed response: " + parsedResponse.toString());
//                if (userInterface != null) {
//                    userInterface.displayResponse(parsedResponse.toString());
//                }
//
//            } catch (SocketTimeoutException e) {
//                System.out.println("Request timed out: No response from server within the timeout period.");
//                if (userInterface != null) {
//                    userInterface.displayResponse("Request timed out: No response from server.");
//                }
//            } catch (IOException e) {
//                if (userInterface != null) {
//                    userInterface.displayResponse("Error receiving response: " + e.getMessage());
//                }
//                System.out.println("Error receiving response: " + e.getMessage());
//            }
//        }
//    }





    //发送请求到服务器
    private void sendRequest(String request) {
        try {
            byte[] requestData = request.getBytes();
            DatagramPacket packet = new DatagramPacket(requestData, requestData.length, serverAddress, serverPort);
            System.out.println("Sending request: " + request);
            socket.send(packet);

            // 启动线程接收服务器响应
            new Thread(new ResponseListener()).start();
            System.out.println("sendRequest done");
        } catch (IOException e) {
            if (userInterface != null) {
                userInterface.displayResponse("Error sending request: " + e.getMessage());
            }
            System.out.println("Error sending request: " + e.getMessage());
        }
    }


    //监听服务器响应
    private class ResponseListener implements Runnable {
        @Override
        public void run() {
            try {
                // 设置超时，单位为毫秒
                socket.setSoTimeout(60000);  // 超时时间为10秒

                byte[] buffer = new byte[1024];
                DatagramPacket responsePacket = new DatagramPacket(buffer, buffer.length);
                System.out.println("Waiting for server response...");
                socket.receive(responsePacket);  // 接收服务器的响应

                String response = new String(responsePacket.getData(), 0, responsePacket.getLength());
                System.out.println("Received response from server: " + response);

                // 将响应结果显示在用户界面上
                if (userInterface != null) {
                    userInterface.displayResponse("Response: " + response);
                }

            } catch (SocketTimeoutException e) {
                System.out.println("Request timed out: No response from server within the timeout period.");
                if (userInterface != null) {
                    userInterface.displayResponse("Request timed out: No response from server.");
                }
            } catch (IOException e) {
                if (userInterface != null) {
                    userInterface.displayResponse("Error receiving response: " + e.getMessage());
                }
            }
        }
    }
}
