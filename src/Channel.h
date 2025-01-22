#pragma once

#include <sys/epoll.h>

class Epoll;

class Channel {
public:
    Channel(Epoll* _epoll, int _fd);
    ~Channel();

    void enable_reading();

    int get_fd();

    uint32_t get_events();

    uint32_t get_revents();
    void set_revents(uint32_t _ev);

    bool get_in_epoll();

    void set_in_epoll();

private:
    Epoll* epoll;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool in_epoll;
};
