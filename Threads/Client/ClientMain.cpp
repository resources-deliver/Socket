#include "ClientHeader.hpp"

int main() {
    // 创建客户端对象，连接到指定服务器
    Client client("127.0.0.1", 8080);

    // 连接服务器
    bool isConnected = client.Connect();
    if(!isConnected){
        std::cerr << "连接服务器失败!" << std::endl;
        return -1;
    }

    // 与服务器通信
    client.Communicate();

    std::cout << "客户端关闭..." << std::endl;
    return 0;
}
