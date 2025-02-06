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

Server::Server(EventLoop* event_loop)
    : _event_loop(event_loop), _accetpor(nullptr) {
    _accetpor = new Acceptor(_event_loop);
    std::function<void(Socket*)> callback = std::bind(&Server::new_connection, this, std::placeholders::_1);
    _accetpor->set_new_connection_callback(callback);
}

Server::~Server() {
    delete _accetpor;
}

void Server::new_connection(Socket* server_socket) {
    Connection* connection = new Connection(_event_loop, server_socket);
    std::function<void(Socket*)> callback = std::bind(&Server::delete_connection, this, std::placeholders::_1);
    connection->set_delete_connection_callback(callback);
    _connections[server_socket->get_fd()] = connection;
}

void Server::delete_connection(Socket* server_socket) {
    Connection* connection = _connections[server_socket->get_fd()];
    _connections.erase(server_socket->get_fd());
    delete connection;
}
