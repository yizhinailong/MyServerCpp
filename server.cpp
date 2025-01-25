#include "Server.h"

#include "EventLoop.h"

int main() {
    EventLoop* event_loop = new EventLoop();
    Server* server = new Server(event_loop);

    event_loop->loop();

    return 0;
}
