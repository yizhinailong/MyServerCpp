#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
public:
    Server(EventLoop* main_reactor);
    ~Server();

    void new_connection(Socket* server_socket);
    void delete_connection(int sockfd);

private:
    EventLoop* _main_reactor;
    Acceptor* _accetpor;
    std::map<int, Connection*> _connections;
    std::vector<EventLoop*> _sub_reactors;
    ThreadPool* _thread_pool;
};
