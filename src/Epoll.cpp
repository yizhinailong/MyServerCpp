#include "Epoll.h"

#include <string.h>
#include <unistd.h>

#include "Channel.h"
#include "util.h"

#define MAX_EVENTS 1024

Epoll::Epoll()
    : _epfd(-1), _events(nullptr) {
    _epfd = epoll_create1(0);
    errif(_epfd == -1, "epoll create error\n");
    _events = new epoll_event[MAX_EVENTS];
    memset(_events, 0, sizeof(*_events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (_epfd != -1) {
        close(_epfd);
        _epfd = -1;
    }
    delete[] _events;
}

void Epoll::add_fd(int fd, uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error\n");
}

// std::vector<epoll_event> Epoll::poll(int timeout) {
//     std::vector<epoll_event> active_events;

//     int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
//     errif(nfds == -1, "epoll wait error\n");
//     for (int i = 0; i < nfds; i++) {
//         active_events.push_back(events[i]);
//     }

//     return active_events;
// }

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activte_events;

    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error\n");
    for (int i = 0; i < nfds; i++) {
        Channel* channel = (Channel*)_events[i].data.ptr;
        channel->set_revents(_events[i].events);
        activte_events.push_back(channel);
    }

    return activte_events;
}

void Epoll::update_channel(Channel* channel) {
    int fd = channel->get_fd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->get_events();
    if (!channel->get_in_epoll()) {
        errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error\n");
        channel->set_in_epoll();
    } else {
        errif(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error\n");
    }
}
