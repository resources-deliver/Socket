#ifndef SERVER_HEADER_HPP
#define SERVER_HEADER_HPP

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
    bool SendData(int acceptSocketFd, const char* data, int len);  // 发送带4字节头部数据包的数据
    int RecvData(int acceptSocketFd, char* buf, int bufsize);  // 接收带4字节头部数据包的数据
    void Close();  // 关闭Socket
};

// 服务端类
class Server : public Socket{
public:
    Server(const std::string& ip = "", int port = 8080);  // 构造函数
    ~Server() override;  // 析构函数

public:
    bool Init(int backlog = 128);  // 初始化服务器（创建socket、绑定、监听）
    int AcceptClient();  // 接受客户端连接
    bool Communicate(int acceptSocketFd);  // 与客户端通信（使用accept返回的socket）

private:
    int port_;  // 服务端端口
    std::string ip_;  // 服务端IP地址
};

#endif // SERVER_HEADER_HPP
