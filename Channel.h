#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Epoll.h"
#include "Socket.h"

#define CHAN_READBUFCAP 1024

class Epoll;

class Channel {
private:
    int fd_ = -1; // channel's socket desc (view)
    Epoll *ep_ = nullptr; // pointer to the epoll instance which has the channel (view)
    bool inepl_ = false; // whether this channel is on the epoll tree
    uint32_t events_ = 0; // the events need to be monitored
    uint32_t revents_ = 0; // happened events
    std::function<void()> readcallback_;

public:
    // ctors and dtors
    Channel(Epoll *ep, int fd);
    
    ~Channel();

    // get the socket fd
    int get_fd();

    // using ET mode
    void use_ET();

    // let epoll_wait to monitor fd_'s read read event
    void enable_reading();

    // set inepl_ = true
    void set_inepl();

    // set revents_
    void setrevent(uint32_t ev);

    // return value of inepl_
    bool get_inepl();

    // get events_
    uint32_t get_events();

    // get revents_
    uint32_t get_revents();

    //handle events returned from epoll_wait
    void handle_event();

    // handle new connection request from listen channel
    void newconnection(Socket *servsock);

    // handle client message from comm channel
    void onmessage();

    // set the callback function
    void set_readcallback(std::function<void()> fn);
};