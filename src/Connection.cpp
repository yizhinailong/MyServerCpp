#include "Connection.h"

#include <errno.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop* event_loop, Socket* socket)
    : _event_loop(event_loop), _socket(socket), _channel(nullptr) {
    _channel = new Channel(_event_loop, _socket->get_fd());
    std::function<void()> callback = std::bind(&Connection::echo, this, _socket->get_fd());
    _channel->set_callback(callback);
    _channel->enable_reading();
}

Connection::~Connection() {
    delete _channel;
    delete _socket;
}

void Connection::echo(int sockfd) {
    char buf[READ_BUFFER];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));

        if (bytes_read > 0) {
            std::cout << "message from sockfd" << sockfd << " : " << buf << std::endl;
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) {
            std::cout << "client interrupt normally, please waitting" << std::endl;
            continue;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::cout << "finish reading once, errno : " << errno << std::endl;
            break;
        } else if (bytes_read == 0) {
            std::cout << "EOF, client sockfd " << sockfd << " disconnection" << std::endl;
            _delete_connection_callback(_socket);
            break;
        }
    }
}

void Connection::set_delete_connection_callback(std::function<void(Socket*)> delete_connection_callback) {
    _delete_connection_callback = delete_connection_callback;
}
