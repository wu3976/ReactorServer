#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddr.h"


int create_nonblocking_socket();

// socket encapsulation
class Socket {
private:
    const int fd_;
public:
    Socket(int fd);
    
    ~Socket();

    // get socket descriptor
    int get_fd() const;

    // set socket SO_REUSEADDR on
    void setreuseaddr(bool on);

    // set socket SO_REUSEPORT on
    void setreuseport(bool on);

    // set socket TCP_NODELAY on
    void settcpnodelay(bool on);

    // set socket SO_KEEPALIVE on
    void setkeepalive(bool on);

    //-----------encapsulate different methods------------
    void bind_addr(const InetAddr &servaddr);

    void listen_conn(int backlog=128);

    int accept_nbconn(InetAddr &clientaddr_out);
};