#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// 服务端IP选择(固定与通用)
struct server_ip_type{
    const char* ipaddress = "192.168.162.128";    // 服务端IP地址(小端)
    in_addr_t network_ipaddress = INADDR_ANY;     // 服务端IP地址(大端)
};

// 接受客户端连接信息
struct accept_info{
    struct sockaddr_in cliaddr;
    int accept_ret;
};

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

// 绑定socket
int bind_socket(int& socket_ret, int& port, struct server_ip_type& iptype){
    int network_protocol = AF_INET;  // 网络协议
    int network_port = htons(port);  // 服务端端口(大端)
    
    // 设置端口复用
    int opt = 1;
    if(setsockopt(socket_ret, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        perror("setsockopt error");
        return -1;
    }
    
    // 绑定服务端IP地址与端口Port
    struct sockaddr_in seraddr;
    seraddr.sin_family = network_protocol;
    seraddr.sin_port = network_port;
    seraddr.sin_addr.s_addr = iptype.network_ipaddress;
    // inet_pton()函数将IP地址转换为网络字节序
    // int netipaddress = inet_pton(
    //     network_protocol, 
    //     iptype.ipaddress, 
    //     &seraddr.sin_addr.s_addr
    // );
    socklen_t seraddr_len = sizeof(seraddr);
    // 绑定socket
    int bind_ret = bind(socket_ret, (struct sockaddr*)&seraddr, seraddr_len);
    if(bind_ret == -1){
        perror("bind error");
        return -1;
    }
    return bind_ret;
    /* INADDR_ANY（默认本机网卡）, 一般用于本地的绑定操作, 地址为0.0.0.0（类似于127.0.0.1） */
}

// 监听socket
int listen_socket(int socket_ret, int once_connection_max){
    // 设置监听
    int listen_ret = listen(socket_ret, once_connection_max);
    if(listen_ret == -1){
        perror("listen error");
        return -1;
    }
    return listen_ret;
}

// 接受客户端连接
accept_info accept_connect(int socket_ret){
    struct accept_info info;
    socklen_t cliaddr_len = sizeof(info.cliaddr);
    // 阻塞等待连接
    info.accept_ret = accept(socket_ret, (struct sockaddr*)&info.cliaddr, &cliaddr_len);
    if(info.accept_ret == -1){
        perror("accept connect error");
        return accept_info();
    }
    return info;
}

// 打印客户端信息
void cli_information(struct accept_info& info){
    char ip[24] = {0};
    int network_protocol = AF_INET;            // 网络协议
    int network_port = info.cliaddr.sin_port;  // 客户端端口(大端)
    int port = ntohs(network_port);            // 客户端端口(小端)
    const void* network_ipaddress = &info.cliaddr.sin_addr.s_addr;     // 客户端IP地址(大端)
    const char* ipaddress = inet_ntop(
        network_protocol, 
        network_ipaddress, 
        ip, 
        sizeof(ip)
    );  // 客户端IP地址(小端)
    printf("客户端的IP地址: %s, 端口: %d\n", ipaddress, port);
}

// 与客户端通信
void cli_communication(struct accept_info& info){
    while(1){
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(info.accept_ret, buf, sizeof(buf));
        if(len > 0){
            printf("客户端say: %s\n", buf);
            write(info.accept_ret, buf, len);
        }
        else if(len  == 0){
            printf("客户端断开了连接...\n");
            break;
        }
        else{
            perror("read");
            break;
        }
    }
}

int main(){
    // 创建socket
    int socket_ret = create_socket();
    if(socket_ret == -1) return -1;

    // 绑定服务器IP与端口Port
    int port = 8080;
    struct server_ip_type iptype;
    int bind_ret = bind_socket(socket_ret, port, iptype);
    if(bind_ret == -1) return -1;

    // 设置监听
    int once_connection_max = 128;
    int listen_ret = listen_socket(socket_ret, once_connection_max);
    if(listen_ret == -1) return -1;

    // 阻塞等待并接受客户端连接
    struct accept_info info = accept_connect(socket_ret);
    if(info.accept_ret == -1) return -1;
    cli_information(info);

    // 和客户端通信
    cli_communication(info);

    // 关闭socket
    close(info.accept_ret);
    close(socket_ret);
    return 0;
}