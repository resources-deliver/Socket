#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// 创建socket
int create_socket(){
    int network_protocol = AF_INET;             // 网络协议
    int transmision_protocol = SOCK_STREAM;     // 传输协议
    int protocol = 0;                           // TCP协议
    int socket_ret = socket(network_protocol, transmision_protocol, protocol);
    if(socket_ret == -1){
        perror("socket error");
        return -1;
    }
    return socket_ret;
    /* AF_INET（IPV4）  AF_INET6（IPV6） */
    /* SOCK_STREAM（流式TCP）  SOCK_DGRAM（数据报UDP） */
}

// 连接服务器
int connect_server(int& socket_ret, int& port, const char* ipaddress){
    int network_protocol = AF_INET;  // 网络协议
    int network_port = htons(port);  // 客户端端口(大端)
    // 绑定客户端IP地址与端口Port
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = network_protocol;
    cliaddr.sin_port = network_port;
    int net_ipaddress = inet_pton(network_protocol, ipaddress, &cliaddr.sin_addr.s_addr);
    if(net_ipaddress == -1){
        perror("inet_pton error");
        return -1;
    }
    // 连接服务器
    int connect_ret = connect(socket_ret, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    if(connect_ret == -1){
        perror("connect error");
        return -1;
    }
    return connect_ret;
}

// 和服务端通信
void ser_communication(int& socket_ret){
    int number = 0;
    while(1){
        // 发送数据
        char buf[1024];
        sprintf(buf, "你好, 服务器...%d\n", number++);
        write(socket_ret, buf, strlen(buf)+1);
        
        // 接收数据
        memset(buf, 0, sizeof(buf));
        int len = read(socket_ret, buf, sizeof(buf));
        if(len > 0){
            printf("服务器say: %s\n", buf);
        }
        else if(len  == 0){
            printf("服务器断开了连接...\n");
            break;
        }
        else{
            perror("read");
            break;
        }
        sleep(1);   // 每隔1s发送一条数据
    }
}

int main(){
    // 创建socket
    int socket_ret = create_socket();
    if(socket_ret == -1) return -1;

    // 连接服务器
    int port = 8080;
    const char* ipaddress = "192.168.162.128";
    int connect_ret = connect_server(socket_ret, port, ipaddress);
    if(connect_ret == -1) return -1;

    // 和服务器端通信
    ser_communication(socket_ret);

    // 关闭socket
    close(socket_ret);
    return 0;
}