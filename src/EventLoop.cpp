#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"

EventLoop::EventLoop()
    : _epoll(nullptr), _quite(false) {
    _epoll = new Epoll();
}

EventLoop::~EventLoop() {
    delete _epoll;
}

void EventLoop::loop() {
    while (!_quite) {
        std::vector<Channel*> channels;
        channels = _epoll->poll();
        for (auto it = channels.begin(); it != channels.end(); it++) {
            (*it)->handle_event();
        }
    }
}

void EventLoop::update_channel(Channel* channel) {
    _epoll->update_channel(channel);
}
