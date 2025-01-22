#include "Socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "InetAddress.h"
#include "util.h"

Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error\n");
}

Socket::Socket(int _fd) : fd(_fd) {
    errif(fd == -1, "socket create error\n");
}

Socket::~Socket() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* addr) {
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error\n");
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::set_nonblocking() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr) {
    int client_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(client_sockfd == -1, "socket accept error\n");
    return client_sockfd;
}

int Socket::get_fd() {
    return fd;
}
