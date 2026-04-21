#include "ServerHeader.hpp"

// 初始化相关变量
Socket::Socket():sockfd_(-1), is_connected_(false){}

// 释放相关资源
Socket::~Socket(){
    // 关闭Socket
    Close();
}

// 发送带4字节头部数据包的数据
bool Socket::SendData(int acceptSocketFd, const char* data, int len){
    // 检查输入参数和连接状态
    if(data == nullptr || len <= 0 || acceptSocketFd == -1){
        return false;  // ......发送失败
    }

    // 先写入4字节的长度头
    int net_len = htonl(len);  // ......将数据长度转换(网络字节序)
    int writeHeader = write(acceptSocketFd, &net_len, 4);
    if(writeHeader != 4){  // .......如果写入长度头失败
        perror("Send data head error!");
        return false;  // ......发送失败
    }

    // 再写入实际数据
    int writeData = write(acceptSocketFd, data, len);
    if(writeData != len){  // ......如果写入实际数据失败
        perror("Send data error!");
        return false;  // ......发送失败
    }

    // 发送成功
    return true;
}

// 接收带4字节头部数据包的数据
int Socket::RecvData(int acceptSocketFd, char* buf, int bufsize){
    // 检查输入参数和连接状态
    if(buf == nullptr || bufsize <= 0 || acceptSocketFd == -1){
        return -1;  // ......接收失败
    }

    // 先读取4字节的长度头
    int net_len = 0;
    int readHeader = read(acceptSocketFd, &net_len, 4);
    if(readHeader != 4){  // ......如果读取长度头失败
        if(readHeader == 0){  // ......如果对方关闭连接
            return 0;  // ......对方关闭连接
        }
        perror("Recv data head error!");
        return -1;  // ......接收失败
    }

    // 检查数据长度是否超过缓冲区大小
    int len = ntohl(net_len);  // ......将网络字节序转换(主机字节序)
    if(len > bufsize){  // ......如果数据长度超过缓冲区大小
        std::cerr << "Data too large: " << len << " > " << bufsize << std::endl;
        return -1;  // ......接收失败
    }

    // 再读取实际数据
    memset(buf, 0, bufsize);  // ......清空缓冲区
    int total = 0;  // ......累计读取的字节数
    while(total < len){
        int readData = read(acceptSocketFd, buf + total, len - total);
        if(readData <= 0){  // ......如果读取实际数据失败
            if(readData == 0){  // .....如果对方关闭连接
                return 0;  // ......对方关闭连接
            }
            perror("Recv data error!");
            return -1;  // ......接收失败
        }
        total += readData;  // ......累计读取的字节数
    }

    // 接收成功
    return len;
}

// 关闭Socket
void Socket::Close(){
    // 如果socket未关闭
    if(sockfd_ != -1){
        close(sockfd_);  // ......关闭Socket
        sockfd_ = -1;  // ......将socket文件描述符置为-1
        is_connected_ = false;  // .....将连接状态置为false
    }
}