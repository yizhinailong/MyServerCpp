#include "Connection.h"

#include <errno.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop* event_loop, Socket* socket)
    : _event_loop(event_loop), _socket(socket), _channel(nullptr) {
    _channel = new Channel(_event_loop, _socket->get_fd());
    std::function<void()> callback = std::bind(&Connection::echo, this, _socket->get_fd());
    _channel->set_callback(callback);
    _channel->enable_reading();
    _read_buffer = new Buffer();
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
            _read_buffer->append(buf, bytes_read);
        } else if (bytes_read == -1 && errno == EINTR) {
            std::cout << "client interrupt normally, please waitting" << std::endl;
            continue;
        } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::cout << "finish reading once, message from client "
                      << sockfd << " : " << _read_buffer->c_str() << std::endl;
            errif(write(sockfd, _read_buffer->c_str(), _read_buffer->size()) == -1, "socket write error");
            _read_buffer->clear();
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
