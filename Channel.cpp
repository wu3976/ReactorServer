#include "Channel.h"
#include "InetAddr.h"
#include "Socket.h"
#include <sys/epoll.h>
/*
class Channel {
private:
    int fd_ = -1; // channel's socket desc (view)
    Epoll *ep_ = nullptr; // pointer to the epoll instance which has the channel (view)
    bool inepl_ = false; // whether this channel is on the epoll tree
    uint32_t events_ = 0; // the events need to be monitored
    uint32_t revents_ = 0; // happened events

public:
    // ctors and dtors
    Channel(Epoll *ep, int fd);
    
    ~Channel();

    // get the socket fd
    int get_fd();

    // using ET mode
    void use_ET();

    // let epoll_wait to monitor fd_'s read read event
    void enable_reading();

    // set inepl_ = true
    void set_inepl();

    // set revents_
    void setrevent(uint32_t ev);

    // return value of inepl_
    bool get_inepl();

    // get events_
    uint32_t get_events();

    // get revents_
    uint32_t get_revents();

};
*/

Channel::Channel(Epoll *ep, int fd, bool islisten) : ep_(ep), fd_(fd), islisten_(islisten){}

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

void Channel::handle_event(Socket *servsock) {
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (islisten_) { // listenfd have pending connection
            InetAddr clientaddr;
            Socket *clientsock = new Socket(servsock->accept_nbconn(clientaddr));
            printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",
                    clientsock->get_fd(), clientaddr.get_ip(), clientaddr.get_port());
            Channel *clichannel = new Channel(ep_, clientsock->get_fd(), false);
            clichannel->use_ET();
            clichannel->enable_reading();
                    
        } else {
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