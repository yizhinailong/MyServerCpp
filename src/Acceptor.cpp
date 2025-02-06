#include "Acceptor.h"

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
}

Acceptor::~Acceptor() {
    delete _socket;
    delete _addr;
    delete _acceptor_channel;
}

void Acceptor::accept_connection() {
    _new_connection_callback(_socket);
}

void Acceptor::set_new_connection_callback(std::function<void(Socket*)> new_connection_callback) {
    _new_connection_callback = new_connection_callback;
}
