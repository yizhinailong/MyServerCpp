#include "Epoll.h"

#include <string.h>
#include <unistd.h>

#include "util.h"

#define MAX_EVENTS 1024

Epoll::Epoll() : epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error\n");
    events = new epoll_event[MAX_EVENTS];
    memset(events, 0, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::add_fd(int fd, uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error\n");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> active_events;

    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error\n");
    for (int i = 0; i < nfds; i++) {
        active_events.push_back(events[i]);
    }

    return active_events;
}
