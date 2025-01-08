#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

int main() {
    // 创建TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 初始化服务器地址结构
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    // 设置服务器地址家族为Internet地址族
    server_addr.sin_family = AF_INET;
    // 设置服务器IP地址为本地回环地址
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 设置服务器端口号为8888
    server_addr.sin_port = htons(8888);

    // 将服务器地址绑定到套接字上
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 监听套接字，准备接收客户端连接
    listen(sockfd, SOMAXCONN);

    // 初始化客户端地址结构
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    std::memset(&client_addr, 0, sizeof(client_addr));
    // 接受客户端连接请求
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);

    // 输出客户端的信息
    std::cout << "new client fd " << client_sockfd << " ! IP: " << inet_ntoa(client_addr.sin_addr)
              << " Port: " << ntohs(client_addr.sin_port) << std::endl;

    return 0;
}
