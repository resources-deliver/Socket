# Socket 单线程通信程序

## 简介

基于TCP协议的单线程Socket通信程序，采用面向对象设计，实现客户端与服务器端的双向通信。使用C++11标准编写，通过Socket基类派生出Server和Client子类，实现代码复用。

### 特点
- 基于TCP协议的Socket通信
- 面向对象设计，通过继承实现代码复用
- 带长度头的数据包协议，确保数据完整性
- 单线程实现，一次只能处理一个客户端
- 支持双向通信
- 支持端口复用

## 结构

```
SingleThread/
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

# 运行客户端
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
- `AcceptClient()` - 接受客户端连接
- `Communicate()` - 与客户端通信

## 功能说明

**服务端**：创建Socket、绑定端口、监听、接受连接、显示客户端信息、双向通信、处理断开。

**客户端**：创建Socket、连接服务器、双向通信（每秒发送消息）、处理断开。

## 通信流程

1. 服务端启动并监听
2. 客户端连接服务器
3. 客户端每秒发送消息
4. 服务器原样返回消息
5. 任一方断开，通信结束

## 数据包协议

前4字节：数据长度（网络字节序）
后续字节：实际数据

## 注意事项

1. 单线程实现，无法同时处理多个客户端
2. 请确保服务器IP和端口配置正确
3. 防火墙可能需要开放8080端口

## 技术要点

- socket/bind/listen/accept/connect
- read/write数据收发
- inet_pton/inet_ntop IP转换
- htons/ntohs/htonl/ntohl字节序转换
- setsockopt端口复用
- 面向对象设计
- C++11 lambda函数