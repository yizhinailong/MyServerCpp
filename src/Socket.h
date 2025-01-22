#pragma once

class InetAddress;

class Socket {
public:
    Socket();
    Socket(int _fd);

    ~Socket();

    void bind(InetAddress* addr);
    void listen();
    void set_nonblocking();

    int accept(InetAddress* addr);

    int get_fd();

private:
    int fd;
};
