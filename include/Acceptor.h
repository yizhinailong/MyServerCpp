#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
public:
    Acceptor(EventLoop* event_loop);
    ~Acceptor();

    void accept_connection();

    void set_new_connection_callback(std::function<void(Socket*)> new_connection_callback);

private:
    EventLoop* _event_loop;
    Socket* _socket;
    InetAddress* _addr;
    Channel* _acceptor_channel;

    std::function<void(Socket*)> _new_connection_callback;
};
