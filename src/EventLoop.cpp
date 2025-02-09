#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.hpp"

EventLoop::EventLoop()
    : _epoll(nullptr), _quite(false), _thread_pool(nullptr) {
    _epoll = new Epoll();
    _thread_pool = new ThreadPool();
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
