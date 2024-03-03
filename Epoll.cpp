#include "Epoll.h"
#include "tools.h"

Epoll::Epoll() {
    epollfd_ = epoll_create(1); 
    if (epollfd_ == -1) {
        perror("epoll_create()");
        SHOWERR("error")
    }
}

Epoll::~Epoll() {
    close(epollfd_);
}

/*
void Epoll::add_fdevent(int fd, uint32_t event_type) {
    epoll_event evt;
    evt.events = event_type;
    evt.data.fd = fd;

    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &evt) == -1) {
        perror("epoll_ctl()");
        SHOWERR("error");
        exit(1);
    }
}*/

void Epoll::updateChannel(Channel *ch) {
    epoll_event evt;
    evt.data.ptr = ch;
    evt.events = ch->get_events();
    if (ch->get_inepl()) {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->get_fd(), &evt) == -1){
            perror("epoll_ctl");
            SHOWERR("error");
            exit(1);
        }
    } else {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->get_fd(), &evt) == -1){
            perror("epoll_ctl");
            SHOWERR("error");
            exit(1);
        }
        ch->set_inepl();
    }
}

// the compiler will optimize the container data copying
/*std::vector<epoll_event> Epoll::loop(int timeout) {
    std::vector<epoll_event> result;
    memset(events_, 0, sizeof(events_));
    int numevent = epoll_wait(epollfd_, events_, EVENTBUFCAP, timeout);
    if (numevent == -1) {
        perror("epoll_wait()");
        SHOWERR("error");
        exit(1);
    }
    if (numevent == 0) {
        printf("epoll_wait timeout\n");
        return result;
    }
    for (int i = 0; i < numevent; i++) {
        result.push_back(events_[i]);
    }
    return result;
}*/

std::vector<Channel *> Epoll::loop(int timeout) {
    std::vector<Channel *> channels;
    memset(events_, 0, sizeof(events_));
    int numevent = epoll_wait(epollfd_, events_, EVENTBUFCAP, timeout);
    if (numevent == -1) {
        perror("epoll_wait()");
        SHOWERR("error");
        exit(1);
    }
    if (numevent == 0) {
        printf("epoll_wait timeout\n");
        return channels;
    }
    for (int i = 0; i < numevent; i++) {
        Channel *c = (Channel *)events_[i].data.ptr;
        c->setrevent(events_[i].events);
        channels.push_back(c);
    }
    return channels;
}

