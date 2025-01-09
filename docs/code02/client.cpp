#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

int main() {
    // 创建一个TCP套接字
    // AF_INET指定使用IPv4协议
    // SOCK_STREAM指定使用面向连接的Socket类型
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检查套接字创建是否成功
    errif(sockfd == -1, "socket create error");

    // 初始化服务器地址结构
    struct sockaddr_in server_addr;
    // 清零服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));

    // 设置服务器地址结构的成员变量
    server_addr.sin_family = AF_INET; // 使用IPv4地址族
    // 将字符串形式的IP地址转换为网络字节序的二进制形式
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将主机字节序的端口号转换为网络字节序
    server_addr.sin_port = htons(8888);

    // 尝试连接到服务器
    // 这里将服务器地址结构转换为 sockaddr* 类型，因为connect函数需要这种类型
    // 添加错误处理，如果连接失败则输出错误信息并退出程序
    errif(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket connect error");

    // 无限循环以持续通信
    while (true) {
        // 初始化读写缓冲区
        char buf[1024];
        // 缓冲区清零
        memset(&buf, 0, sizeof(buf));
        // 从标准输入读取一行数据
        scanf("%s", buf);
        // 向服务器发送缓冲区内容，记录发送的字节数
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // 发送失败则提示并跳出循环
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        // 重置缓冲区以准备接收数据
        memset(&buf, 0, sizeof(buf));
        // 从服务器读取数据到缓冲区，记录读取的字节数
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        // 根据读取结果进行不同处理
        if (read_bytes > 0) {
            // 成功读取到数据，输出该消息
            printf("message send to server: %s\n", buf);
        } else if (read_bytes == 0) {
            // 服务器关闭连接，提示并跳出循环
            printf("server socket disconnected!\n");
            break;
        } else if (read_bytes == -1) {
            // 读取错误，关闭socket并输出错误信息后退出程序
            close(sockfd);
            errif(true, "socket read error");
        }
    }

    // 关闭socket
    close(sockfd);

    return 0;
}
