#include "Server.h"

#include <errno.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

#define READ_BUFFER 1024

Server::Server(EventLoop* event_loop)
    : _event_loop(event_loop) {
    Socket* server_socket = new Socket();
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8888);
    server_socket->bind(server_addr);
    server_socket->listen();
    server_socket->set_nonblocking();

    Channel* server_channel = new Channel(_event_loop, server_socket->get_fd());
    std::function<void()> callback = std::bind(&Server::new_connection, this, server_socket);
    server_channel->set_callback(callback);
    server_channel->enable_reading();
}

Server::~Server() {
}

void Server::handle_read_event(int sockfd) {
    char buf[READ_BUFFER];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));

        if (bytes_read > 0) {
            std::cout << "message from sockfd " << sockfd << ":" << buf << std::endl;
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            std::cout << "client interrupt normally, please waiting" << std::endl;
            continue;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::cout << "finish reading once, errno : " << errno << std::endl;
            break;
        } else if (bytes_read == 0) {
            std::cout << "EOF, client sockfd " << sockfd << "disconnection" << std::endl;
            close(sockfd);
            break;
        }
    }
}

void Server::new_connection(Socket* server_socket) {
    InetAddress* client_addr = new InetAddress();
    Socket* client_socket = new Socket(server_socket->accept(client_addr));

    std::cout << "new client socket: " << client_socket->get_fd()
              << "IP: " << inet_ntoa(client_addr->addr.sin_addr)
              << "Port: " << htons(client_addr->addr.sin_port)
              << std::endl;

    client_socket->set_nonblocking();
    Channel* client_channel = new Channel(_event_loop, client_socket->get_fd());
    std::function<void()> callback = std::bind(&Server::handle_read_event, this, client_socket->get_fd());
    client_channel->set_callback(callback);
    client_channel->enable_reading();
}
