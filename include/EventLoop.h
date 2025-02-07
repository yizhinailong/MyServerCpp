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

    void add_thread(std::function<void()> thread);

private:
    Epoll* _epoll;
    bool _quite;
    ThreadPool* _thread_pool;
};
