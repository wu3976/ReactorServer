#include "Eventloop.h"

/*class Eventloop {
private:
    Epoll *ep_;
public:
    // ctor and dtor
    Eventloop(); // create Epoll instance in ctor
    ~Eventloop(); // destroy resources.

    // run the event loop
    void run();
};*/

Eventloop::Eventloop() : ep_(new Epoll) {} 

Eventloop::~Eventloop() {
    delete ep_;
}

void Eventloop::run() {
    while (true) {
        //std::vector<epoll_event> events;
        std::vector<Channel *> channels = ep_->loop();
        for (auto &ch : channels) 
        {
            ch->handle_event();
        }
    }
}

Epoll *Eventloop::get_ep() {
    return ep_;
}