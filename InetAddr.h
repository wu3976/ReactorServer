#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

// socket protocal & address class
class InetAddr
{
private:
    sockaddr_in addr_;
public:
    // ctors abd dtors
    InetAddr(const std::string &ip, uint16_t port);
    
    InetAddr(const sockaddr_in addr);
    
    ~InetAddr();

    // 3 getters
    const char *get_ip() const;
    
    uint16_t get_port() const;
    
    const sockaddr *get_addrref() const;
};

