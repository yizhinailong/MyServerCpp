#include "Channel.h"

#include "Epoll.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* event_loop, int fd)
    : _event_loop(event_loop), _fd(fd), _events(0), _revents(0), _in_epoll(false) {}

Channel::~Channel() {}

void Channel::enable_reading() {
    _events = EPOLLIN | EPOLLET;
    _event_loop->update_channel(this);
}

int Channel::get_fd() {
    return _fd;
}

uint32_t Channel::get_events() {
    return _events;
}

uint32_t Channel::get_revents() {
    return _revents;
}

void Channel::set_revents(uint32_t ev) {
    _revents = ev;
}

bool Channel::get_in_epoll() {
    return _in_epoll;
}

void Channel::set_in_epoll() {
    _in_epoll = true;
}

void Channel::handle_event() {
    _event_loop->add_thread(_callback);
}

void Channel::set_callback(std::function<void()> callback) {
    _callback = callback;
}
