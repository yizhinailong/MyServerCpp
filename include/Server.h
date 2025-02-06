#pragma once
#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server {
public:
    Server(EventLoop* event_loop);
    ~Server();

    void new_connection(Socket* server_socket);
    void delete_connection(Socket* server_socket);

private:
    EventLoop* _event_loop;
    Acceptor* _accetpor;
    std::map<int, Connection*> _connections;
};
