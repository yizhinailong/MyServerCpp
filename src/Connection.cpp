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
    : _event_loop(event_loop), _socket(socket), _channel(nullptr), _read_buffer(nullptr) {
    _channel = new Channel(_event_loop, _socket->get_fd());
    _channel->enable_read();
    _channel->use_ET();
    std::function<void()> callback = std::bind(&Connection::echo, this, _socket->get_fd());
    _channel->set_read_callback(callback);
    _read_buffer = new Buffer();
}

Connection::~Connection() {
    delete _channel;
    delete _socket;
    delete _read_buffer;
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
            send(sockfd);
            _read_buffer->clear();
            break;
        } else if (bytes_read == 0) {
            std::cout << "EOF, client sockfd " << sockfd << " disconnection" << std::endl;
            _delete_connection_callback(sockfd);
            break;
        } else {
            std::cout << "Connection reset by peer!" << std::endl;
            _delete_connection_callback(sockfd);
            break;
        }
    }
}

void Connection::set_delete_connection_callback(std::function<void(int)> delete_connection_callback) {
    _delete_connection_callback = delete_connection_callback;
}

void Connection::send(int sockfd) {
    char buffer[_read_buffer->size()];
    strcpy(buffer, _read_buffer->c_str());
    int data_size = _read_buffer->size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write = write(sockfd, buffer + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        data_size -= bytes_write;
    }
}
