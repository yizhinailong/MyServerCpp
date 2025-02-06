#include "Server.h"

#include <errno.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Acceptor.h"
#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

#define READ_BUFFER 1024

Server::Server(EventLoop* event_loop)
    : _event_loop(event_loop), _accetpor(nullptr) {
    _accetpor = new Acceptor(_event_loop);
    std::function<void(Socket*)> callback = std::bind(&Server::new_connection, this, std::placeholders::_1);
    _accetpor->set_new_connection_callback(callback);
}

Server::~Server() {
    delete _accetpor;
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
