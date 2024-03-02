#include "InetAddr.h"
#include <string.h>


InetAddr::InetAddr(const std::string &ip, uint16_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddr::InetAddr(const sockaddr_in addr) : addr_(addr){}

InetAddr::~InetAddr() {}

const char *InetAddr::get_ip() const {
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddr::get_port() const {
    return ntohs(addr_.sin_port);
}

const sockaddr *InetAddr::get_addrref() const {
    return (const sockaddr *)&addr_;
}