# Socket 多线程通信程序

## 简介

基于TCP协议的多线程Socket通信程序，采用面向对象设计，实现服务器端同时处理多个客户端连接。使用C++11标准编写，通过Socket基类派生出Server和Client子类，实现代码复用。与单线程版本相比，多线程版本能够同时为多个客户端提供服务，提高并发处理能力。

### 特点
- 基于TCP协议的Socket通信
- 面向对象设计，通过继承实现代码复用
- 带长度头的数据包协议，确保数据完整性
- **多线程实现，同时处理多个客户端**
- 支持双向通信
- 支持端口复用
- 每个客户端由独立线程处理

## 项目结构

```
Threads/
├── Client/              # 客户端代码
│   ├── ClientHeader.hpp
│   ├── ClientMain.cpp
│   ├── ClientSocket.cpp
│   └── ClientCommunication.cpp
├── Server/              # 服务器代码
│   ├── ServerHeader.hpp
│   ├── ServerMain.cpp
│   ├── ServerSocket.cpp
│   └── ServerCommunication.cpp
├── Compiled/            # 编译输出
└── README.md
```

## 编译运行

```bash
# 编译服务端
g++ -std=c++11 -Wall Server/*.cpp -o Compiled/server

# 编译客户端
g++ -std=c++11 -Wall Client/*.cpp -o Compiled/client

# 运行服务端
./Compiled/server

# 运行客户端（可启动多个）
./Compiled/client
```

默认配置：服务端端口8080，绑定所有网卡；客户端连接127.0.0.1:8080（可在代码中修改）。

## 类设计

### Socket 基类
- `SendData()` - 发送带4字节长度头的数据包
- `RecvData()` - 接收带4字节长度头的数据包
- `Close()` - 关闭Socket

### Client 类
- `Connect()` - 连接服务器
- `Communicate()` - 与服务器通信（每秒发送一条消息）

### Server 类
- `Init()` - 初始化服务器（创建、绑定、监听）
- `Run()` - 运行服务器（接受连接并创建线程）
- `Communicate()` - 与客户端通信

## 功能说明

**服务端**：创建Socket、绑定端口、监听、接受连接、为每个客户端创建独立线程、显示客户端信息、双向通信、处理断开。

**客户端**：创建Socket、连接服务器、双向通信（每秒发送消息）、处理断开。

## 通信流程

1. 服务端启动并监听
2. 客户端连接服务器
3. 服务端接受连接并创建独立线程
4. 客户端每秒发送消息
5. 服务器线程原样返回消息
6. 任一方断开，该客户端通信结束
7. 服务端继续等待新连接

## 多线程实现

```cpp
// 为每个客户端创建新线程处理通信
client_threads_.emplace_back([this, acceptSocket](){
    this->Communicate(acceptSocket);
});
client_threads_.back().detach();  // 分离线程
```

每个客户端连接由独立线程处理，使用`detach()`分离线程，客户端断开后线程自动结束。

## 数据包协议

前4字节：数据长度（网络字节序）
后续字节：实际数据

## 注意事项

1. 多线程实现，可同时处理多个客户端
2. 每个客户端由独立线程处理，资源自动释放
3. 请确保服务器IP和端口配置正确
4. 防火墙可能需要开放8080端口
5. 编译时需要添加`-pthread`选项
6. 大量客户端可能消耗较多资源

## 技术要点

- socket/bind/listen/accept/connect
- read/write数据收发
- inet_pton/inet_ntop IP转换
- htons/ntohs/htonl/ntohl字节序转换
- setsockopt端口复用
- 面向对象设计
- C++11 lambda函数
- C++11 std::thread多线程
- std::vector管理线程