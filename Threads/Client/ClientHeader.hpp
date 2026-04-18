#ifndef CLIENT_HEADER_HPP
#define CLIENT_HEADER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>

// Socket基类 - 提供通用的Socket功能
class Socket{
public:
    Socket();  // 默认构造函数
    virtual ~Socket();  // 虚析构函数
    Socket(const Socket&) = delete;  // 禁止拷贝构造
    Socket& operator=(const Socket&) = delete;  // 禁止拷贝赋值

public:  // 未启用
    int GetSocketFd()const{ return sockfd_; };  // 获取socket文件描述符
    bool IsConnected()const{ return is_connected_; };  // 获取连接状态

protected:
    int sockfd_;  // socket文件描述符
    bool is_connected_;  // 连接状态

protected:
    bool SendData(const char* data, int len);  // 发送带4字节头部数据包的数据
    int RecvData(char* buf, int bufsize);  // 接收带4字节头部数据包的数据
    void Close();  // 关闭Socket
};

// 客户端类
class Client : public Socket{
public:
    Client(const std::string& ip = "127.0.0.1", int port = 8080);  // 构造函数
    ~Client() override;  // 析构函数

public:
    bool Connect();  // 连接服务器
    void Communicate();  // 与服务器通信

public:  // 未启用
    bool SendMessage(const std::string& msg);  // 发送消息
    bool ReceiveMessage(std::string& msg);  // 接收消息

private:
    std::string server_ip_;  // 服务器IP地址
    int server_port_;  // 服务器端口
};

#endif // CLIENT_HEADER_HPP
