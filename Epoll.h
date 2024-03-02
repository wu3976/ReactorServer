#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>

// a epoll class
class Epoll {
private:
    const constexpr static int EVENTBUFCAP = 10;

    int epollfd_ = -1;
    epoll_event events_[EVENTBUFCAP];

public:
    //--------------ctors and dtors------------------

    // create an epoll instance
    Epoll();

    ~Epoll();

    // add socket and its event type to be waited
    void add_fdevent(int fd, uint32_t event_type);

    // wait until events occur
    std::vector<epoll_event> loop(int timeout=-1);
};