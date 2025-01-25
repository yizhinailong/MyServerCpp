#include "Socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "InetAddress.h"
#include "util.h"

Socket::Socket()
    : _fd(-1) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(_fd == -1, "socket create error\n");
}

Socket::Socket(int fd)
    : _fd(fd) {
    errif(_fd == -1, "socket create error\n");
}

Socket::~Socket() {
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}

void Socket::bind(InetAddress* addr) {
    errif(::bind(_fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error\n");
}

void Socket::listen() {
    errif(::listen(_fd, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::set_nonblocking() {
    fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr) {
    int client_sockfd = ::accept(_fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(client_sockfd == -1, "socket accept error\n");
    return client_sockfd;
}

int Socket::get_fd() {
    return _fd;
}
