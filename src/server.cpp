#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"

#define READ_BUFFER 1024

void handle_read_event(int sockfd) {
    char buf[READ_BUFFER];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));

        if (bytes_read > 0) {
            printf("message from sockfd %d : %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            printf("client interrupt normally, please waiting\n");
            continue;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("finish reading once, errno : %d\n", errno);
            break;
        } else if (bytes_read == 0) {
            printf("EOF, client sockfd %d disconnection\n", sockfd);
            close(sockfd);
            break;
        }
    }
}

int main() {
    /* create server socket */
    Socket* server_socket = new Socket();
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8888);
    server_socket->bind(server_addr);
    server_socket->listen();

    /* create epoll */
    Epoll* epoll = new Epoll();
    server_socket->set_nonblocking();
    epoll->add_fd(server_socket->get_fd(), EPOLLIN | EPOLLET);

    while (true) {
        std::vector<epoll_event> events = epoll->poll();
        int nfds = events.size();

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_socket->get_fd()) {
                InetAddress* client_addr = new InetAddress();
                Socket* client_socket = new Socket(server_socket->accept(client_addr));

                printf("new client socket: %d IP: %s Port: %d\n", client_socket->get_fd(), inet_ntoa(client_addr->addr.sin_addr),
                       htons(client_addr->addr.sin_port));

                client_socket->set_nonblocking();
                epoll->add_fd(client_socket->get_fd(), EPOLLIN | EPOLLET);
            } else if (events[i].events | EPOLLIN) {
                handle_read_event(events[i].data.fd);
            } else {
                printf("something else happened\n");
            }
        }
    }

    delete server_socket;
    delete server_addr;

    return 0;
}
