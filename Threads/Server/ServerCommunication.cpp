#include "ServerHeader.hpp"

// 初始化相关变量
Server::Server(const std::string& ip, int port)
    :Socket(), port_(port), ip_(ip){}

// 释放相关资源
Server::~Server(){
    // 关闭socket
    Close();
}

// 初始化服务器
bool Server::Init(int backlog){
    // 创建socket
    // AF_INET——网络协议  SOCK_STREAM——传输协议  protocol=0——TCP协议
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_ == -1){
        perror("Create socket error!");
        return false;  // ......初始化服务器失败
    }

    // 设置端口复用
    std::function<bool(bool)> ReSetPort = [this](bool on){
        int opt = on ? 1 : 0;
        int setSocket = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if(setSocket == -1){
            perror("Port reuse error!");
            return false;  // ......设置端口复用失败
        }
        return true;  // ......设置端口复用成功
    };
    bool reusePort = ReSetPort(true);
    if(!reusePort){
        Close();    // ......关闭socket
        return false;  // ......初始化服务器失败
    }

    // 设置服务器网络协议、IP和端口信息
    struct sockaddr_in serverAddr;  // ......存放服务器ip和端口信息
    memset(&serverAddr, 0, sizeof(serverAddr));  // ......初始化存放器
    serverAddr.sin_family = AF_INET;  // ......存放网络协议
    serverAddr.sin_port = htons(port_);  // ......存放端口信息(网络字节序)
    if(ip_.empty()){  // 如果未传入IP参数给构造函数
        serverAddr.sin_addr.s_addr = INADDR_ANY;  // ......存放IP地址信息(网络字节序)
    }
    else{
        int ipChange = inet_pton(  // ......存放IP地址信息(网络字节序)
            AF_INET,
            ip_.c_str(),
            &serverAddr.sin_addr.s_addr
        );
        if(ipChange <= 0){  // ......IP地址转换失败
            perror("Change hostIP to networkIP error");
            Close();  // ......关闭socket
            return false;  // ......初始化服务器失败
        }
    }

    // 绑定服务器网络协议、IP和端口信息
    int bindSocket = bind(sockfd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(bindSocket == -1){  // ......绑定失败
        perror("Bind socket error");
        Close();  // ......关闭socket
        return false;  // ......初始化服务器失败
    }

    // 开始监听
    int listenSocket = listen(sockfd_, backlog);
    if(listenSocket == -1){  // ......监听失败
        perror("Listen socket error");
        Close();  // ......关闭socket
        return false;  // ......初始化服务器失败
    }

    // 返回绑定成功并开始监听
    is_connected_ = true;
    return true;
}

// 运行服务器（接受客户端连接并创建线程处理通信）
void Server::Run(){
    // 循环接受客户端连接
    while(true){
        // 接收客户端信息
        struct sockaddr_in clientAddr;  // ......存放客户端ip和端口信息
        socklen_t clientAddr_len = sizeof(clientAddr);

        // 阻塞等待连接
        int acceptSocket = accept(sockfd_, (struct sockaddr*)&clientAddr, &clientAddr_len);
        if(acceptSocket == -1){
            perror("Accept connection error!");
            continue;  // ......接受失败，继续监听
        }

        // 输出客户端IP和端口信息
        std::function<void()> PrintClientInfo = [&clientAddr](){
            char ip_str[INET_ADDRSTRLEN] = {0};
            // 客户端IP地址信息(主机字节序)
            const char* ipChange = inet_ntop(
                AF_INET,
                &clientAddr.sin_addr,
                ip_str,
                sizeof(ip_str)
            );
            if(ipChange == nullptr){  // ......IP地址转换失败
                perror("IP地址:网络字节序(大端)——>主机字节序(小端)失败");
                return;  // ......输出客户端信息失败
            }

            // 客户端端口信息(主机字节序)
            int portChange = ntohs(clientAddr.sin_port);
            if(portChange == -1){  // ......端口信息转换失败
                perror("Port信息:网络字节序(大端)——>主机字节序(小端)失败");
                return;  // ......输出客户端信息失败
            }

            // 输出客户端IP和端口信息(主机字节序)
            std::cout << "客户端连接: IP=" << ip_str << ", Port=" << portChange << std::endl;
        };
        PrintClientInfo();  // ......调用包装后的匿名函数

        // 为每个客户端创建新线程处理通信
        client_threads_.emplace_back([this, acceptSocket](){
            this->Communicate(acceptSocket);
        });
        client_threads_.back().detach();  // 分离线程，让其独立运行
    }
}

// 与客户端通信
bool Server::Communicate(int acceptSocketFd){
    char buf[1024];  // ......存放客户端发送的数据
    while(true){
        memset(buf, 0, sizeof(buf));  // .......清空缓冲区
        // (读取)接收客户端发送的数据
        int len = RecvData(acceptSocketFd, buf, sizeof(buf));
        if(len > 0){
            std::cout << "客户端say: " << buf << std::endl;
            // (写入)发送回显数据
            if(!SendData(acceptSocketFd, buf, len)){
                break;
            }
        }
        else if(len == 0){
            std::cout << "客户端断开了连接..." << std::endl;
            break;
        }
        else{
            perror("Recv data error!");
            break;
        }
    }
    close(acceptSocketFd);  // 关闭连接的客户端socket
    return true;
}
