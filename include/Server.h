#pragma once

class EventLoop;
class Socket;
class Acceptor;

class Server {
public:
    Server(EventLoop* event_loop);
    ~Server();

    void handle_read_event(int sockfd);
    void new_connection(Socket* server_socket);

private:
    EventLoop* _event_loop;
    Acceptor* _accetpor;
};
