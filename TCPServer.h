#pragma once
#include "Eventloop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include <string>

class TCPServer {
private:
    Eventloop evtlp_;
    Acceptor *acptor_;  

public:
    // ctor and dtors 
    TCPServer(const std::string &ip, const uint16_t port);
    ~TCPServer();

    // run the event loop of evtlp
    void start();
};