#include "ClientHeader.hpp"

// 初始化相关变量
Client::Client(const std::string& ip, int port)
    :Socket(), server_ip_(ip), server_port_(port){}

// 释放相关资源
Client::~Client(){
    // 关闭Socket
    Close();
}

// 创建Socket并连接服务器
bool Client::Connect(){
    // 创建socket
    // AF_INET——网络协议  SOCK_STREAM——传输协议  protocol=0——TCP协议
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_ == -1){
        perror("Create socket error!");
        return false;  // ......连接失败
    }

    // 添加服务器网络协议、ip和端口信息
    struct sockaddr_in serverAddr;  // ......存放服务器ip和端口信息
    memset(&serverAddr, 0, sizeof(serverAddr));  // ......初始化存放器
    serverAddr.sin_family = AF_INET;  // ......存放网络协议
    serverAddr.sin_port = htons(server_port_);  // ......存放端口信息(网络字节序)
    int ipChange = inet_pton(  // ......存放IP地址信息(网络字节序)
        AF_INET,
        server_ip_.c_str(),
        &serverAddr.sin_addr.s_addr
    );
    if(ipChange <= 0){
        perror("Change hostIP to networkIP error");
        Close();  // ......关闭Socket
        return false;  // ......连接失败
    }

    // 连接服务器
    int connectServer = connect(sockfd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(connectServer == -1){
        perror("Connect sever error");
        Close();  // ......关闭Socket
        return false;  // ......连接失败
    }

    // 返回连接成功
    is_connected_ = true;
    std::cout << "已连接到服务器,IP:" << server_ip_ << " Port:" << server_port_ << std::endl;
    return true;
}

// 与服务器通信
void Client::Communicate(){
    // 检查连接状态
    if(!is_connected_){
        std::cerr << "未连接到服务器!" << std::endl;
        return;
    }

    // 发送与接收数据
    int number = 0;  // ......消息计数器
    while(true){  // ......不断发送与接收数据
        // 发送数据
        char buf[1024];  // ......存放数据
        snprintf(buf, sizeof(buf), "你好, 服务器...%d", number++);
        int bufferlen = strlen(buf) + 1;

        // 发送数据失败则退出循环
        bool sendData = SendData(buf, bufferlen);
        if(!sendData){
            break;
        }

        // 接收数据
        memset(buf, 0, sizeof(buf));  // 清空缓冲区
        int len = RecvData(buf, sizeof(buf));
        if(len > 0){
            std::cout << "服务器say: " << buf << std::endl;
        }
        else if(len == 0){
            std::cout << "服务器断开了连接..." << std::endl;
            break;
        }
        else{
            break;
        }

        // 每隔1秒发送一条数据
        sleep(1);
    }
}

// 发送消息
bool Client::SendMessage(const std::string& msg){
    // 检查连接状态
    if(!is_connected_) return false;
    bool sendResult = SendData(msg.c_str(), msg.length() + 1);
    return sendResult;
}

// 接收消息
bool Client::ReceiveMessage(std::string& msg){
    // 检查连接状态
    if(!is_connected_) return false;

    char buf[1024];
    int len = RecvData(buf, sizeof(buf));
    if(len > 0){
        msg = buf;
        return true;
    }
    return false;
}
