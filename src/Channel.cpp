#include "Channel.h"

#include "Epoll.h"

Channel::Channel(Epoll* _epoll, int _fd) : epoll(_epoll), fd(_fd), events(0), revents(0), in_epoll(false) {}

Channel::~Channel() {}

void Channel::enable_reading() {
    events = EPOLLIN | EPOLLET;
    epoll->update_channel(this);
}

int Channel::get_fd() {
    return fd;
}

uint32_t Channel::get_events() {
    return events;
}

uint32_t Channel::get_revents() {
    return revents;
}

void Channel::set_revents(uint32_t _ev) {
    revents = _ev;
}

bool Channel::get_in_epoll() {
    return in_epoll;
}

void Channel::set_in_epoll() {
    in_epoll = true;
}
