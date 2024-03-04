#include "Channel.h"
#include "InetAddr.h"
#include "Socket.h"
#include <sys/epoll.h>

Channel::Channel(Epoll *ep, int fd) : ep_(ep), fd_(fd){}

// do not close ep_ and fd_
Channel::~Channel() {}

int Channel::get_fd() {
    return fd_;
}

void Channel::use_ET() {
    events_ |= EPOLLET;
}

void Channel::enable_reading(){
    events_ |= EPOLLIN;
    ep_->updateChannel(this);
}

void Channel::set_inepl() {
    inepl_ = true;
}

void Channel::setrevent(uint32_t ev) {
    revents_ = ev;
}

bool Channel::get_inepl() {
    return inepl_;
}

uint32_t Channel::get_events() {
    return events_;
}

uint32_t Channel::get_revents() {
    return revents_;
}

void Channel::handle_event() {
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        readcallback_();
    }         
    else if (revents_ & EPOLLRDHUP) {
        printf("client: fd=%d disconnected with EPOLLRDHUP\n", fd_);
        close(fd_); // automatically delete event in eventbuf
    } 
    else if (revents_ & EPOLLOUT) {}
    else {
        printf("client: fd=%d error with unknown event\n", fd_);
        close(fd_); // automatically delete event in eventbuf
    }
}

void Channel::newconnection(Socket *servsock) {
    InetAddr clientaddr;
    Socket *clientsock = new Socket(servsock->accept_nbconn(clientaddr));
    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",
            clientsock->get_fd(), clientaddr.get_ip(), clientaddr.get_port());
    Channel *clichannel = new Channel(ep_, clientsock->get_fd());
    clichannel->set_readcallback(std::bind(&Channel::onmessage, clichannel));
    clichannel->use_ET();
    clichannel->enable_reading();
}

void Channel::onmessage() {
    char buffer[CHAN_READBUFCAP];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesred = recv(fd_, buffer, CHAN_READBUFCAP, 0);
        if (bytesred == 0) { // client closes the socket
            printf("client: fd=%d disconnected with 0 byte\n", fd_);
            close(fd_); // automatically delete event in eventbuf
            break;
        } else if (bytesred == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { break; }
            perror("recv()");
            exit(1);
        } else {
            printf("Received from fd=%d: %s\n", fd_, buffer);
            send(fd_, buffer, strlen(buffer), 0);
        }
    }
}

void Channel::set_readcallback(std::function<void()> fn) {
    readcallback_ = fn;
}