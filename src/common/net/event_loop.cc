#include <common/net/event_loop.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

namespace sails {
namespace common {
namespace net {

//const int EventLoop::INIT_EVENTS = 1000;

EventLoop::EventLoop() {
    memset(anfds, 0, 1000*sizeof(struct ANFD));
}

EventLoop::~EventLoop() {
    
}

void EventLoop::init() {
    epollfd = epoll_create(10);
    assert(epollfd > 0);
    for(int i = 0; i < INIT_EVENTS; i++) {
	anfds[i].isused = 0;
	anfds[i].fd = 0;
	anfds[i].events = 0;
	anfds[i].next = NULL;
    }
}

bool EventLoop::event_ctl(OperatorType op, struct event* ev) {

    if(ev->fd > max_events) {
	// malloc new evetns and anfds
	return false;
    }

    struct event *e = (struct event*)malloc(sizeof(struct event));
    e->fd = ev->fd;
    e->events = ev->events;
    e->cb = ev->cb;
    e->next = NULL;

    int fd = e->fd;

    int need_add_to_epoll = false;
    if(anfds[fd].isused == 1) {
	if((anfds[fd].events | e->events) != anfds[fd].events) {
	    need_add_to_epoll = true;
	    struct event *t_e = anfds[fd].next;
	    while(t_e->next != NULL) {
		t_e = t_e->next;
	    }
	    t_e->next = e;
	}
    }else {
	anfds[fd].isused = 1;
	need_add_to_epoll = true;
	anfds[fd].next = e;
    }

    if(need_add_to_epoll) {
	struct epoll_event epoll_ev;
	short events = 0;
	epoll_ev.data.fd = ev->fd;
	if(ev->events & Event_READ) {
	    events = events | EPOLLIN | EPOLLET;
	}
	if(ev->events & Event_WRITE) {
	    events = events | EPOLLOUT;
	}
	epoll_ev.events = events;
	if (epoll_ctl(epollfd, op, ev->fd, &epoll_ev) == -1) {
	    perror("epoll_ctl: listen_sock");
	    return false;
	}
    }

    struct event *temp = anfds[fd].next;
    int new_events = 0;
    while(temp != NULL) {
	new_events |= temp->events;
	temp = temp->next;
    }
    anfds[fd].events = new_events;
    
    return true;
}

void EventLoop::start_loop() {
    for(;;) {
	int nfds = epoll_wait(epollfd, events, max_events, -1);
	if(nfds == -1) {
	    perror("start_loop, epoll wait");
	    exit(EXIT_FAILURE);
	}
	for(int n = 0; n < nfds; ++n) {
	    int fd = events[n].data.fd;
	    if(anfds[fd].isused == 1) {
		// find events for fd and callback
		int ev = 0;
		if(events[n].events & EPOLLIN) {
		    ev |= Event_READ;
		}
		if(events[n].events & EPOLLOUT) {
		    ev |= Event_WRITE;
		}
		process_event(fd, ev);
	    }
	}
    }
}

void EventLoop::process_event(int fd, int events) {
    if(fd < 0 || fd > max_events) {
	return;
    }
    if(anfds[fd].isused != 1) {
	return;
    }
    if(anfds[fd].events & events) {
	struct event* io_w = anfds[fd].next;
	while(io_w != NULL) {
	    if(io_w->events & events && io_w->fd == fd) {
		io_w->cb(io_w, io_w->events);
	    }
	    io_w = io_w->next;
	}
    }
}

} // namespace net
} // namespace common
} // namespace sails

