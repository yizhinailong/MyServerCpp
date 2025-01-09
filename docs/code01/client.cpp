#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>

int main() {
    // 创建一个TCP套接字
    // AF_INET指定使用IPv4协议
    // SOCK_STREAM指定使用面向连接的Socket类型
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 初始化服务器地址结构
    struct sockaddr_in server_addr;
    // 使用memset将server_addr结构体清零，避免未初始化的垃圾值
    memset(&server_addr, 0, sizeof(server_addr));

    // 设置服务器地址结构的成员变量
    server_addr.sin_family = AF_INET; // 使用IPv4地址族
    // 将字符串形式的IP地址转换为网络字节序的二进制形式
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 将主机字节序的端口号转换为网络字节序
    server_addr.sin_port = htons(8888);

    // 尝试连接到服务器
    // 这里将服务器地址结构转换为 sockaddr* 类型，因为connect函数需要这种类型
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    return 0;
}
