#include "Channel.h"

#include <sys/epoll.h>
#include <unistd.h>

#include "EventLoop.h"
#include "Socket.h"

Channel::Channel(EventLoop* event_loop, int fd)
    : _event_loop(event_loop), _fd(fd), _events(0), _ready(0), _in_epoll(false) {
}

Channel::~Channel() {
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}

void Channel::handle_event() {
    if (_ready & (EPOLLIN | EPOLLPRI)) {
        _read_callback();
    }
    if (_ready & EPOLLOUT) {
        _write_callback();
    }
}

void Channel::enable_read() {
    _events |= EPOLLIN | EPOLLPRI;
    _event_loop->update_channel(this);
}

int Channel::get_fd() {
    return _fd;
}

uint32_t Channel::get_events() {
    return _events;
}

uint32_t Channel::get_ready() {
    return _ready;
}

bool Channel::get_in_epoll() {
    return _in_epoll;
}

void Channel::set_in_epoll(bool in) {
    _in_epoll = in;
}

void Channel::use_ET() {
    _events |= EPOLLET;
    _event_loop->update_channel(this);
}

void Channel::set_ready(uint32_t ev) {
    _ready = ev;
}

void Channel::set_read_callback(std::function<void()> callback) {
    _read_callback = callback;
}
