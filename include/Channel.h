#pragma once

#include <functional>
#include <sys/epoll.h>

class EventLoop;

class Channel {
public:
    Channel(EventLoop* epoll, int fd);
    ~Channel();

    void enable_reading();

    int get_fd();

    uint32_t get_events();

    uint32_t get_revents();
    void set_revents(uint32_t ev);

    bool get_in_epoll();

    void set_in_epoll();

    void handle_event();

    void set_callback(std::function<void()> callback);

private:
    EventLoop* _epoll;
    int _fd;
    uint32_t _events;
    uint32_t _revents;
    bool _in_epoll;
    std::function<void()> _callback;
};
