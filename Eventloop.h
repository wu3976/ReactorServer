#pragma once
#include "Epoll.h"
#include "Channel.h"

class Epoll;
class Channel;
class Eventloop {
private:
    Epoll *ep_;
public:
    // ctor and dtor
    Eventloop(); // create Epoll instance in ctor
    ~Eventloop(); // destroy resources.

    // run the event loop
    void run();

    Epoll *get_ep();
    void updatechannel(Channel *ch);
};