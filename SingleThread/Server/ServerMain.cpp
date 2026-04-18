#include "ServerHeader.hpp"
#include <iostream>

int main(){
    // 创建服务器对象，绑定所有网卡，端口8080
    Server server("", 8080);

    // 初始化服务器(创建socket、绑定bind、监听listen)
    bool initServer = server.Init();
    if(!initServer){
        std::cerr << "服务器初始化失败!" << std::endl;
        return -1;
    }
    std::cout << "服务器启动成功，等待客户端连接..." << std::endl;

    // 接受客户端连接
    int client_sockfd = server.AcceptClient();
    if(client_sockfd == -1){
        std::cerr << "接受客户端连接失败!" << std::endl;
        return -1;
    }

    // 与客户端通信
    bool communicate = server.Communicate(client_sockfd);
    if(!communicate){
        std::cerr << "与客户端通信失败!" << std::endl;
        return -1;
    }

    // 关闭服务器
    close(client_sockfd);
    std::cout << "服务器关闭..." << std::endl;
    return 0;
}
