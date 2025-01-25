#pragma once

#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll {
public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);

    void update_channel(Channel* channel);

    // std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> poll(int timeout = -1);

private:
    int _epfd;
    struct epoll_event* _events;
};
