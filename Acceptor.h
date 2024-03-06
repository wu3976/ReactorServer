#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddr.h"
#include "Eventloop.h"

class Acceptor {

private:
    Eventloop *loop_; // JUST A VIEW of an Eventloop
    Socket *servsock_; // socket for the listening
    Channel *acceptchannel_; // channel of the acceptor

public:
    Acceptor(Eventloop *evtlp, const std::string &ip, const uint16_t port);
    ~Acceptor();

};