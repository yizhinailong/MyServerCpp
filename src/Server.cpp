#include "Server.h"

#include <errno.h>
#include <functional>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.hpp"

Server::Server(EventLoop* main_reactor)
    : _main_reactor(main_reactor), _accetpor(nullptr) {
    _accetpor = new Acceptor(_main_reactor);
    std::function<void(Socket*)> callback = std::bind(&Server::new_connection, this, std::placeholders::_1);
    _accetpor->set_new_connection_callback(callback);

    int size = std::thread::hardware_concurrency();
    _thread_pool = new ThreadPool(size);
    for (int i = 0; i < size; i++) {
        _sub_reactors.push_back(new EventLoop());
    }

    for (int i = 0; i < size; i++) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, _sub_reactors[i]);
        _thread_pool->add(sub_loop);
    }
}

Server::~Server() {
    delete _accetpor;
    delete _thread_pool;
}

void Server::new_connection(Socket* server_socket) {
    if (server_socket->get_fd() != -1) {
        int random = server_socket->get_fd() % _sub_reactors.size();
        Connection* connection = new Connection(_sub_reactors[random], server_socket);
        std::function<void(int)> callback = std::bind(&Server::delete_connection, this, std::placeholders::_1);
        connection->set_delete_connection_callback(callback);
        _connections[server_socket->get_fd()] = connection;
    }
}

void Server::delete_connection(int sockfd) {
    if (sockfd != -1) {
        auto it = _connections.find(sockfd);
        if (it != _connections.end()) {
            Connection* connection = _connections[sockfd];
            _connections.erase(sockfd);
            delete connection;
        }
    }
}
