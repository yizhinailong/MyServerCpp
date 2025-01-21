#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

#define MAX_EVENTS  1024 // 最大事件数
#define READ_BUFFER 1024 // 缓存的大小

void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

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

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll ctreate error");

    struct epoll_event events[MAX_EVENTS], ev;
    memset(&events, 0, sizeof(events));
    memset(&ev, 0, sizeof(ev));

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    set_nonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    // 与客户端进行通信的循环
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) {
                /* new client connection */
                struct sockaddr_in client_addr;
                memset(&client_addr, 0, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockfd == -1, "socket accept error");

                printf("new client fd %d , IP %s, Port %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

                memset(&ev, 0, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                set_nonblocking(client_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
            } else if (events[i].events & EPOLLIN) {
                /* new event connection */
                char buf[READ_BUFFER];
                while (true) {
                    memset(&buf, 0, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));

                    if (bytes_read > 0) {
                        printf("message from client fd %d : %s", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if (bytes_read == -1 && errno == EINTR) {
                        printf("client interrupts normally, waiting to connection");
                        continue;
                    } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                        printf("finish reading once, errno : %d", errno);
                        break;
                    } else if (bytes_read == 0) {
                        printf("EOF, client fd %d disconnection", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            } else {
                printf("something else happened\n");
            }
        }
    }

    // 关闭服务器监听套接字
    close(sockfd);

    return 0;
}
