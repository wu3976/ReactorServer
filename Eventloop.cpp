#include "Eventloop.h"

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

void Eventloop::updatechannel(Channel *ch) {
    ep_->updateChannel(ch);
}