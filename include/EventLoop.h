#pragma once

class Channel;
class Epoll;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void update_channel(Channel* channel);

private:
    Epoll* _epoll;
    bool _quite;
};
