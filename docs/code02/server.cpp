#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

int main() {
    // 创建TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 填加错误处理
    errif(sockfd == -1, "socket create error");

    // 初始化服务器地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // 设置服务器地址家族为Internet地址族
    server_addr.sin_family = AF_INET;
    // 设置服务器IP地址为本地回环地址
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 设置服务器端口号为8888
    server_addr.sin_port = htons(8888);

    // 将服务器地址绑定到套接字上, 添加错误处理
    errif(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket bind error");

    // 监听套接字，准备接收客户端连接
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    // 初始化客户端地址结构
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    // 接受客户端连接请求
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    // 添加错误处理
    errif(client_sockfd == -1, "socket accept error");

    // 输出客户端的信息
    printf("new client fd %d ! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // 与客户端进行通信的循环
    while (true) {
        char buf[1024];               // 缓冲区，用于存储接收的数据
        memset(&buf, 0, sizeof(buf)); // 清零缓冲区

        // 从客户端读取数据
        ssize_t read_bytes = read(client_sockfd, buf, sizeof(buf));

        // 根据读取到的数据字节数进行处理
        if (read_bytes > 0) { // 数据读取成功
            printf("message from client fd %d: %s\n", client_sockfd, buf);
            // 将接收到的数据回传给客户端
            write(client_sockfd, buf, sizeof(buf));
        } else if (read_bytes == 0) {  // 对方关闭连接
            printf("client fd %d disconnected\n", client_sockfd);
            close(client_sockfd);      // 关闭客户端套接字
            break;                     // 退出循环
        } else if (read_bytes == -1) { // 读取错误
            close(client_sockfd);      // 关闭客户端套接字
            errif(true, "socket read error");
        }
    }

    // 关闭服务器监听套接字
    close(sockfd);

    return 0;
}
