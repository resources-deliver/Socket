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

    // 运行服务器（接受客户端连接并创建线程处理通信）
    server.Run();

    // 关闭服务器（理论上不会到达这里）
    std::cout << "服务器关闭..." << std::endl;
    return 0;
}
