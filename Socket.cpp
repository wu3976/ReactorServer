#include "Socket.h"
#include "tools.h"

int create_nonblocking_socket() {
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd == -1) {
        perror("socket()");
        SHOWERR("error")
        exit(1);
    }
    return fd;
}

Socket::Socket(int fd) : fd_(fd) {

}

Socket::~Socket() {
    close(fd_);
}

int Socket::get_fd() const{
    return fd_;
}

void Socket::setreuseaddr(bool on) {
    int option_val = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &option_val,
            static_cast<socklen_t>(sizeof(option_val)));    
}

void Socket::setreuseport(bool on) {
    int option_val = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &option_val,
            static_cast<socklen_t>(sizeof(option_val)));  
}

void Socket::settcpnodelay(bool on) {
    int option_val = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &option_val,
            static_cast<socklen_t>(sizeof(option_val)));  
}

void Socket::setkeepalive(bool on) {
    int option_val = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &option_val,
            static_cast<socklen_t>(sizeof(option_val)));  
}

void Socket::bind_addr(const InetAddr &servaddr) {
    if (bind(fd_, servaddr.get_addrref(), sizeof(sockaddr)) == -1) {
        perror("bind()");
        SHOWERR("error")
        exit(1);
    }
}

void Socket::listen_conn(int backlog) {
    if (listen(fd_, backlog) == -1) {
        perror("listen()");
        SHOWERR("error")
        exit(1);
    }
}

int Socket::accept_nbconn(InetAddr &clientaddr_out) {
    sockaddr_in peeraddr; 
    socklen_t peeraddrsz = sizeof(peeraddr);
    int commfd = accept4(fd_, (sockaddr *)&peeraddr, &peeraddrsz, SOCK_NONBLOCK);
    clientaddr_out.setaddr(peeraddr);
    return commfd;
}
