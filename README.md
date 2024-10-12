# SC6103_DS
due 2024.10.16 code + report

due 2024.10.19 demonstration

next meeting:
 - 10.10 9:00am
 - 10.12 after DS class

## A Distributed Flight Information System
 - CS Architecture
 - UDP sockets
![CS Communication Flow](cs-communication-flow.png)

```markdown
flight_booking_system/
│
├── client_java/
│   ├── src/
│   │   ├── com/
│   │   │   ├── flightsystem/
│   │   │   │   ├── Client.java                 # 客户端主程序
│   │   │   │   ├── UserInterface.java          # 用户界面管理
│   │   │   │   ├── MessageUtils.java           # 数据编组和解组工具类
│   │   │   │   ├── CallbackHandler.java        # 处理回调的类
│   │   │   │   └── UDPUtils.java               # UDP套接字工具类
│   │   └── resources/
│   │       └── config.properties               # 配置文件，存储服务器地址和端口等信息
│   └── build.gradle                            # Gradle构建脚本
│
├── server/
│   ├── server.c                                # 服务器主程序
│   ├── flight_service.c                        # 航班相关服务实现
│   ├── flight_service.h                        # 航班服务头文件
│   ├── callback_handler.c                      # 处理回调的实现
│   ├── callback_handler.h                      # 回调处理头文件
│   ├── data_storage.c                          # 航班数据存储与管理
│   ├── data_storage.h                          # 数据存储头文件
│   ├── thread_pool.c                           # 线程池管理
│   ├── thread_pool.h                           # 线程池头文件
│   └── Makefile                                # 用于编译服务器代码的Makefile
│
├── common/
│   ├── protocol.h                              # 通信协议头文件，定义请求类型、消息格式等
│   ├── udp_utils.c                             # UDP套接字相关的通用工具函数
│   ├── udp_utils.h                             # 套接字工具头文件
│   └── Makefile                                # 用于编译通用模块的Makefile
│
├── docs/
│   ├── README.md                               # 项目概述和使用说明
│   ├── SYSTEM_ARCHITECTURE.md                  # 系统架构文档
│   ├── CLIENT_GUIDE.md                         # 客户端使用指南
│   └── SERVER_GUIDE.md                         # 服务器部署与运行指南
│
└── Makefile                                    # 顶层Makefile，用于编译整个项目
```

### Server

#### store the information of all flights
```
flight:
{
    flight_id: int
    source_place: variable-length str
    destination_place: variable-length str
    departure_time: {
        year
        month
        day
        hour
        minute
    }
    airfare: float
    seat_availability: int // num of seats available
}
```

**1. 网络接口编程的介绍**

- 接口的定义和目的
- 接口通信的概述（客户端-服务器模型）
- 连程间通信的概念

**2. 接口和端口概述**

- 接口终端的说明
- 将接口绑定到（网络地址，本地端口）的配对
- 连程间通信的详情

**3. C 语言中的接口编程**

- UNIX 系统上的接口编程概述
- 主要系统调用：
    - `socket()`: 创建接口
    - `close()`: 销毁接口
    - `bind()`: 将接口绑定到地址
    - `sendto()` 和 `recvfrom()`: 发送和接收数据
    - `select()`: 检测达到的消息
- 使用 C 语言实现 UDP 通信的示例：
    - 客户端-服务器的消息交换

**4. C 语言中的 TCP 通信**

- TCP 通信的系统调用
- 建立连接（服务器端的 `listen()`，`accept()` 和客户端的 `connect()`）
- 使用 `write()` 和 `read()` 进行数据传输
- TCP 客户端-服务器交互的示例


#### implement services on the flights for remote access by clients
```
1. query_flight_id (source_place, destination_place) {
    if multiple flights match:
        return a list of all
    if no flight matches:
        return an error message
}

2. query_departure_time (flight_id)
    query_airfare (flight_id)
    query_seat_availability (flight_id)
    if flight_id does not exist:
        return an error message

3. make_seat_reservation (flight_id, num_seats) {
    if successful reservation:
        return acknowledgement to client
        update seat_availability on server 
    if incorrect user input (flight_id does not exist or insufficient available for num_seats):
        return an error message
}

4. callback (server & client): monitor_seat_availability (main pre content)

5. two more operations on the flights through client-server communication:
    - one idempotent 选餐
    - one non-idempotent VIP休息室+其他各类需求购买

6. create a new thread to serve each request received

7. The client address is obtained by the server
when it receives a request from a client.
```

要搭建该服务器，请按照以下步骤操作：

### 1. 项目结构设置
根据给定的项目结构，创建文件夹和文件：
```bash
mkdir -p flight_booking_system/server
cd flight_booking_system/server
touch server.c flight_service.c flight_service.h callback_handler.c callback_handler.h data_storage.c data_storage.h thread_pool.c thread_pool.h Makefile
```

### 2. 编写代码
根据提供的框架细化代码。在每个文件中实现相应的功能：

- **server.c**: 主服务器程序，负责监听客户端请求并进行处理。
- **flight_service.c / flight_service.h**: 实现航班查询、航班详细信息和座位预订等服务。
- **callback_handler.c / callback_handler.h**: 用于处理客户端监控服务中的回调请求。
- **data_storage.c / data_storage.h**: 负责航班数据的管理（存储、添加、更新等操作）。
- **thread_pool.c / thread_pool.h**: 提供一个简单的线程池用于处理并发请求。

#### server.c（主程序框架）
主要实现服务器套接字的创建、绑定，以及请求处理的逻辑。

#### flight_service.c / flight_service.h
这里实现所有与航班服务相关的功能，如航班查询、航班详细信息、座位预订等。将`server.c`中关于航班处理的代码迁移到`flight_service.c`中，并使用头文件来声明这些函数，以便在主程序中调用。

例如：
```c
// flight_service.h
#ifndef FLIGHT_SERVICE_H
#define FLIGHT_SERVICE_H

#include <netinet/in.h>

void handle_query_flight(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_query_details(int sockfd, struct sockaddr_in *client_addr, char *buffer);
void handle_reservation(int sockfd, struct sockaddr_in *client_addr, char *buffer);

#endif
```

### 3. 实现Makefile
创建一个`Makefile`用于管理编译过程，支持编译所有源文件并生成可执行文件：
```makefile
CC = gcc
CFLAGS = -Wall -g
OBJ = server.o flight_service.o callback_handler.o data_storage.o thread_pool.o udp_utils.o

all: server

server: $(OBJ)
	$(CC) $(CFLAGS) -o server $(OBJ)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

flight_service.o: flight_service.c flight_service.h
	$(CC) $(CFLAGS) -c flight_service.c

callback_handler.o: callback_handler.c callback_handler.h
	$(CC) $(CFLAGS) -c callback_handler.c

data_storage.o: data_storage.c data_storage.h
	$(CC) $(CFLAGS) -c data_storage.c

thread_pool.o: thread_pool.c thread_pool.h
	$(CC) $(CFLAGS) -c thread_pool.c

udp_utils.o: ../common/udp_utils.c ../common/udp_utils.h
	$(CC) $(CFLAGS) -c ../common/udp_utils.c

clean:
	rm -f *.o server
```
该Makefile支持编译每个模块的源文件，并生成服务器的可执行文件`server`。可以通过执行`make`命令来编译代码。

### 4. 编译和运行
首先进入`server`目录并使用`Makefile`进行编译：
```bash
cd flight_booking_system/server
make
```
如果一切正常，你将看到生成的可执行文件`server`。

然后运行服务器：
```bash
./server
```
这将启动UDP服务器并监听端口8080的客户端请求。

### 5. 测试
你可以使用`client_java`项目中的客户端程序来测试服务器是否正常工作。确保在`client_java/resources/config.properties`中设置正确的服务器IP和端口。

### 总结
通过以上步骤，服务器端的项目已经搭建起来，并且实现了各模块之间的分工协作。这种模块化设计有助于开发和维护，例如，你可以独立开发和测试航班服务模块、回调处理模块等。如果遇到任何问题或需要进一步的解释，请告诉我。

### Client
1. provide an interface that repeatedly asks the user to enter a request and sends the request to the server
 
2. include an option for the user to terminate the client

3. already know the server address and port number

4. Message
    1. self-design format
    2. transmit in byte array
        - marshaling (int / float / str)
        - unmarshalling
```
 #include <netinet/in.h>
 uint32_t htonl(uint32_t hostlong);
 uint32_t ntohl(uint32_t netlong);
```


## Meeting Notes
### 2024.10.06
 1. task decomposition and distribution
    - server side (C) - Gaohan & Ziling
    - client side (Java) - Fanhui & Shuangyue
 2. customized function determination
    - idempotent: choose meal
    - non-idempotent: buy VIP lounge or other additional services

### 2024.10.10


### 2024.10.12
