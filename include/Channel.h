#pragma once

#include <cstdint>
#include <functional>

class EventLoop;
class Socket;

class Channel {
public:
    Channel(EventLoop* event_loop, int fd);
    ~Channel();

    void handle_event();
    void enable_read();

    int get_fd();
    uint32_t get_events();
    uint32_t get_ready();
    bool get_in_epoll();
    void set_in_epoll(bool in = true);
    void use_ET();

    void set_ready(uint32_t ev);
    void set_read_callback(std::function<void()> callback);

private:
    EventLoop* _event_loop;
    int _fd;
    uint32_t _events;
    uint32_t _ready;
    bool _in_epoll;
    std::function<void()> _read_callback;
    std::function<void()> _write_callback;
};
