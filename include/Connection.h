#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
public:
    Connection(EventLoop* event_loop, Socket* socket);
    ~Connection();

    void echo(int sockfd);
    void set_delete_connection_callback(std::function<void(Socket*)> delete_connection_callback);

private:
    EventLoop* _event_loop;
    Socket* _socket;
    Channel* _channel;
    std::function<void(Socket*)> _delete_connection_callback;
    std::string* in_buffer;
    Buffer* _read_buffer;
};
