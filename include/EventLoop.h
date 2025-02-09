#pragma once
#include <functional>

class Channel;
class Epoll;
class ThreadPool;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void update_channel(Channel* channel);

private:
    Epoll* _epoll;
    bool _quite;
    ThreadPool* _thread_pool;
};
