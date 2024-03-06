#include "TCPServer.h"

TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    acptor_ = new Acceptor(&evtlp_, ip, port);
}

TCPServer::~TCPServer() {
    delete acptor_;
}

void TCPServer::start() {
    evtlp_.run();
}