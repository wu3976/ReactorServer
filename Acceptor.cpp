#include "Acceptor.h"

/*
class Acceptor {

private:
    Eventloop *loop_; // just a view of an Eventloop
    Socket *servsock_;
    Channel *acceptchannel_;

public:
    Acceptor(Eventloop *evtlp, const std::string &ip, const uint16_t port);
    ~Acceptor();
    
};
*/

Acceptor::Acceptor(Eventloop *evtlp, const std::string &ip, 
        const uint16_t port) : loop_(evtlp) {
    servsock_ = new Socket(create_nonblocking_socket());
    InetAddr servaddr(ip, port);
    servsock_->setreuseaddr(true);
    servsock_->settcpnodelay(true);
    servsock_->setreuseport(true);
    servsock_->setkeepalive(true);
    servsock_->bind_addr(servaddr);
    servsock_->listen_conn();
    printf("listening connections at %s:%d\n", 
            servaddr.get_ip(), servaddr.get_port());
    
    acceptchannel_ = new Channel(loop_, servsock_->get_fd());
    acceptchannel_->set_readcallback(std::bind(&Channel::newconnection, 
            acceptchannel_, servsock_));
    acceptchannel_->enable_reading();
}

Acceptor::~Acceptor() {
    delete servsock_;
    delete acceptchannel_;
}
