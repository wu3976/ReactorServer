#include "TCPServer.h"

/*
class TCPServer {
private:
    Eventloop evtlp_;

public:
    // ctor and dtors 
    TCPServer(const std::string &ip, const uint16_t port);
    ~TCPServer();
};
*/

TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    Socket *servsock = new Socket(create_nonblocking_socket());
    InetAddr servaddr(ip, port);
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);
    servsock->setkeepalive(true);
    servsock->bind_addr(servaddr);
    servsock->listen_conn();
    printf("listening connections at %s:%d\n", 
            servaddr.get_ip(), servaddr.get_port());
    
    Channel *servchannel = new Channel(&evtlp_, servsock->get_fd());
    servchannel->set_readcallback(std::bind(&Channel::newconnection, 
            servchannel, servsock));
    servchannel->enable_reading();
}

TCPServer::~TCPServer() {

}

void TCPServer::start() {
    evtlp_.run();
}