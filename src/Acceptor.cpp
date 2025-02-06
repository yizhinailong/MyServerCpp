#include "Acceptor.h"

#include <iostream>

#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop* event_loop)
    : _event_loop(event_loop) {
    _socket = new Socket();
    _addr = new InetAddress("127.0.0.1", 8888);
    _socket->bind(_addr);
    _socket->listen();
    _socket->set_nonblocking();
    _acceptor_channel = new Channel(_event_loop, _socket->get_fd());
    std::function<void()> callback = std::bind(&Acceptor::accept_connection, this);
    _acceptor_channel->set_callback(callback);
    _acceptor_channel->enable_reading();
    delete _addr;
}

Acceptor::~Acceptor() {
    delete _socket;
    delete _acceptor_channel;
}

void Acceptor::accept_connection() {
    InetAddress* client_addr = new InetAddress();
    Socket* client_socket = new Socket(_socket->accept(client_addr));

    std::cout << "new client sockfd " << client_socket->get_fd()
              << " IP " << inet_ntoa(client_addr->addr.sin_addr)
              << " Port " << ntohl(client_addr->addr.sin_port)
              << std::endl;

    client_socket->set_nonblocking();
    _new_connection_callback(client_socket);
    delete client_addr;
}

void Acceptor::set_new_connection_callback(std::function<void(Socket*)> new_connection_callback) {
    _new_connection_callback = new_connection_callback;
}
