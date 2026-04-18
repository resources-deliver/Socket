#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// 发送带4字节头部的数据
int send_data(int& sockfd, const char* data, int& len) {
    if (data == NULL || len <= 0) {
        return -1;
    }

    // 将数据长度转换为网络字节序(4字节头部)
    int net_len = htonl(len);

    // 先发送4字节的长度头
    int ret = write(sockfd, &net_len, 4);
    if (ret != 4) {
        perror("send length error");
        return -1;
    }

    // 再发送实际数据
    ret = write(sockfd, data, len);
    if (ret != len) {
        perror("send data error");
        return -1;
    }

    return len;
}

// 接收带4字节头部的数据
int recv_data(int& sockfd, char* buf, int& bufsize) {
    if (buf == NULL || bufsize <= 0) {
        return -1;
    }

    // 先读取4字节的长度头
    int net_len = 0;
    int ret = read(sockfd, &net_len, 4);
    if (ret != 4) {
        if (ret == 0) {
            return 0;  // 对方关闭连接
        }
        perror("recv length error");
        return -1;
    }

    // 将网络字节序转换为主机字节序
    int len = ntohl(net_len);

    // 检查数据长度是否超过缓冲区大小
    if (len > bufsize) {
        fprintf(stderr, "data too large: %d > %d\n", len, bufsize);
        return -1;
    }

    // 读取实际数据
    memset(buf, 0, bufsize);
    int total = 0;
    while (total < len) {
        ret = read(sockfd, buf + total, len - total);
        if (ret <= 0) {
            if (ret == 0) {
                return 0;  // 对方关闭连接
            }
            perror("recv data error");
            return -1;
        }
        total += ret;
    }

    return len;
}

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
    // 设置网络协议和端口
    int network_protocol = AF_INET;
    int network_port = htons(port);
    if(network_port == -1){
        perror("htons error");
        return -1;
    }

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
        int bufferlen = strlen(buf) + 1;
        if(send_data(socket_ret, buf, bufferlen) == -1){
            break;
        }
        
        // 接收数据
        int buffersize = sizeof(buf);
        memset(buf, 0, buffersize);
        int len = recv_data(socket_ret, buf, buffersize);
        if(len > 0){
            printf("服务器say: %s\n", buf);
        }
        else if(len  == 0){
            printf("服务器断开了连接...\n");
            break;
        }
        else{
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