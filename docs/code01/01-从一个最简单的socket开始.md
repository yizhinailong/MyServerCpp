# 01-从一个最简单的socket开始

如果读者之前有计算机网络的基础知识那就更好了，没有也没关系，socket编程非常容易上手。但本教程主要偏向实践，不会详细讲述计算机网络协议、网络编程原理等。想快速入门可以看以下博客，讲解比较清楚、错误较少：
- [计算机网络基础知识总结](https://www.runoob.com/w3cnote/summary-of-network.html)

要想打好基础，抄近道是不可的，有时间一定要认真学一遍谢希仁的《计算机网络》，要想精通服务器开发，这必不可少。

首先在服务器，我们需要建立一个socket套接字，对外提供一个网络通信接口，在Linux系统中这个套接字竟然仅仅是一个文件描述符，也就是一个`int`类型的值！这个对套接字的所有操作（包括创建）都是最底层的系统调用。
> 在这里读者务必先了解什么是Linux系统调用和文件描述符，《现代操作系统》第四版第一章有详细的讨论。如果你想抄近道看博客，C语言中文网的这篇文章讲了一部分：[socket是什么？套接字是什么？](http://c.biancheng.net/view/2123.html)

> Unix哲学KISS：keep it simple, stupid。在Linux系统里，一切看上去十分复杂的逻辑功能，都用简单到不可思议的方式实现，甚至有些时候看上去很愚蠢。但仔细推敲，人们将会赞叹Linux的精巧设计，或许这就是大智若愚。

```cpp
// file: <sys/socket.h>
/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
extern int socket (int __domain, int __type, int __protocol) __THROW;
```

参数：

1. `__domain`: 指定套接字的域（例如，`AF_INET` 表示 IPv4，`AF_INET6` 表示 IPv6）。

2. `__type`

   : 套接字的类型，常见的类型包括：

   - `SOCK_STREAM`：流式套接字，通常用于 TCP 协议。
   - `SOCK_DGRAM`：数据报套接字，通常用于 UDP 协议。

3. `__protocol`: 套接字使用的协议，如果设置为 0，系统会自动选择合适的协议。

返回值：

- 成功时，返回一个新的文件描述符（整数），用于后续的套接字操作。
- 出错时，返回 -1。

```cpp
#include <sys/socket.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    return 0;
}
```

- 第一个参数：IP地址类型，AF_INET表示使用IPv4，如果使用IPv6请使用AF_INET6。
- 第二个参数：数据传输方式，SOCK_STREAM表示流格式、面向连接，多用于TCP。SOCK_DGRAM表示数据报格式、无连接，多用于UDP。
- 第三个参数：协议，0表示根据前面的两个参数自动推导协议类型。设置为IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP和UDP。

对于客户端，服务器存在的唯一标识是一个IP地址和端口，这时候我们需要将这个套接字绑定到一个IP地址和端口上。首先创建一个sockaddr_in结构体
```cpp
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    
    return 0;
}
```
然后使用`bzero`初始化这个结构体，这个函数在头文件`<string.h>`或`<cstring>`中。这里用到了两条《Effective C++》的准则：
> 条款04: 确定对象被使用前已先被初始化。如果不清空，使用gdb调试器查看addr内的变量，会是一些随机值，未来可能会导致意想不到的问题。

> 条款01: 视C++为一个语言联邦。把C和C++看作两种语言，写代码时需要清楚地知道自己在写C还是C++。如果在写C，请包含头文件`<string.h>`。如果在写C++，请包含`<cstring>`。

`bzero`使用：

在 C 语言中，`bzero()` 是一个用于将内存区域的内容设置为零的函数。它通常用于初始化一个字符数组或结构体，将其所有字节置为 0。

函数原型：

```c
/* Set N bytes of S to 0.  */
extern void bzero (void *__s, size_t __n) __THROW __nonnull ((1));
```

参数：

- `s`：指向需要清零的内存区域的指针。
- `n`：要清零的字节数。

返回值：

`bzero()` 不返回任何值。它是 `void` 类型的。

示例代码：

```c
#include <stdio.h>
#include <string.h>

int main() {
    char buffer[10];
    
    // 使用 bzero 将 buffer 中的所有字节设置为 0
    bzero(buffer, sizeof(buffer));

    // 打印 buffer 的内容，查看是否被清零
    for (int i = 0; i < 10; i++) {
        printf("%d ", buffer[i]);
    }

    return 0;
}
```

使用 `memset()` 替代：

```c
#include <stdio.h>
#include <string.h>

int main() {
    char buffer[10];
    
    // 使用 memset 将 buffer 中的所有字节设置为 0
    memset(buffer, 0, sizeof(buffer));

    // 打印 buffer 的内容，查看是否被清零
    for (int i = 0; i < 10; i++) {
        printf("%d ", buffer[i]);
    }

    return 0;
}
```

`memset()` 的语法：

```c
void *memset(void *s, int c, size_t len);
```
设置地址族、IP地址和端口：
```cpp
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    return 0;
}
```
然后将socket地址与文件描述符绑定：
```cpp
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    return 0;
}
```
> 为什么定义的时候使用专用socket地址（sockaddr_in）而绑定的时候要转化为通用socket地址（sockaddr），以及转化IP地址和端口号为网络字节序的`inet_addr`和`htons`等函数及其必要性，在游双《Linux高性能服务器编程》第五章第一节：socket地址API中有详细讨论。

最后我们需要使用`listen`函数监听这个socket端口，这个函数的第二个参数是listen函数的最大监听队列长度，系统建议的最大值`SOMAXCONN`被定义为128。
```cpp
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(sockfd, SOMAXCONN);

    return 0;
}
```
要接受一个客户端连接，需要使用`accept`函数。对于每一个客户端，我们在接受连接时也需要保存客户端的socket地址信息，于是有以下代码：
```cpp
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main() {
    // 创建TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 初始化服务器地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

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
    memset(&client_addr, 0, sizeof(client_addr));
    // 接受客户端连接请求
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);

    // 输出客户端的信息
    printf("new client fd %d ! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return 0;
}
```
要注意和`accept`和`bind`的第三个参数有一点区别，对于`bind`只需要传入server_addr的大小即可，而`accept`需要写入客户端socket长度，所以需要定义一个类型为`socklen_t`的变量，并传入这个变量的地址。另外，`accept`函数会阻塞当前程序，直到有一个客户端socket被接受后程序才会往下运行。

到现在，客户端已经可以通过IP地址和端口号连接到这个socket端口了，让我们写一个测试客户端连接试试：
```cpp
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
```
代码和服务器代码几乎一样：创建一个socket文件描述符，与一个IP地址和端口绑定，最后并不是监听这个端口，而是使用`connect`函数尝试连接这个服务器。

至此，day01的教程已经结束了，进入`code/day01`文件夹，使用make命令编译，将会得到`server`和`client`。输入命令`./server`开始运行，直到`accept`函数，程序阻塞、等待客户端连接。然后在一个新终端输入命令`./client`运行客户端，可以看到服务器接收到了客户端的连接请求，并成功连接。
```bash
new client fd 4 ! IP: 127.0.0.1 Port: 48189
```
但如果我们先运行客户端、后运行服务器，在客户端一侧无任何区别，却并没有连接服务器成功，因为我们day01的程序没有任何的错误处理。

事实上对于如`socket`,`bind`,`listen`,`accept`,`connect`等函数，通过返回值以及`errno`可以确定程序运行的状态、是否发生错误。在day02的教程中，我们会进一步完善整个服务器，处理所有可能的错误，并实现一个echo服务器（客户端发送给服务器一个字符串，服务器收到后返回相同的内容）。

参考代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day01](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day01)