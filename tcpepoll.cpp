#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include "InetAddr.h"
#include "Socket.h"

#define EVENTBUFCAP 10
#define READBUFCAP 1024

using namespace std;

unsigned short parsecmd(int argc, char **argv, string &ipout) {
    if (argc != 3) {
        cout << "usage: ./server <ip> <PORT>" << endl;
        exit(1);
    }
    ipout = argv[1];
    unsigned int port = atoi(argv[2]);
    if (port < 0 || port > 65535) {
        cout << "Iilligal port" << endl;
        exit(1);
    }
    return (unsigned short)port;
}

// create a socket and set to nonblocking
/*
int create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd == -1) {
        perror("socket()");
        exit(1);
    }

    int opt = 1; 
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    
    setsockopt(fd,SOL_SOCKET,TCP_NODELAY   ,&opt,static_cast<socklen_t>(sizeof opt));    
    setsockopt(fd,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt)); 
    setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE   ,&opt,static_cast<socklen_t>(sizeof opt));
    
    return fd;
}

void bind_socket(int fd, string &ip, unsigned short port) {
    InetAddr addr(ip, port);
    if (bind(fd, addr.get_addrref(), sizeof(sockaddr)) == -1) {
        perror("bind()");
        exit(1);
    }
}

void listen_socket(int fd) {
    if (listen(fd, 128) == -1) {
        perror("listen()");
        exit(1);
    }
}*/

void epoll_add_event(int epfd, int eventdesc, int fd) {
    epoll_event evt;
    evt.events = eventdesc;
    evt.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) == -1) {
        perror("epoll_ctl()");
        exit(1);
    }
}

int main(int argc, char **argv) {
    string ip;
    unsigned short port = parsecmd(argc, argv, ip);
    /*
    int listenfd = create_socket();
    bind_socket(listenfd, ip, port);
    listen_socket(listenfd);*/
    Socket servsock(create_nonblocking_socket());
    InetAddr servaddr(ip, port);
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind_addr(servaddr);
    servsock.listen_conn();

    int epfd = epoll_create(1); // epoll instance

    // listen socket events
    epoll_add_event(epfd, EPOLLIN, servsock.get_fd());

    // create a epoll event buffer
    epoll_event eventbuf[EVENTBUFCAP];

    while (true) {
        int numevent = epoll_wait(epfd, eventbuf, EVENTBUFCAP, -1);
        if (numevent == -1) {
            perror("epoll_wait()");
            exit(1);
        }
        if (numevent == 0) {
            cout << "epoll_wait timeout" << endl;
            continue;
        }
        for (int i = 0; i < numevent; i++) {
            if (eventbuf[i].events & (EPOLLIN | EPOLLPRI)) {
                if (eventbuf[i].data.fd == servsock.get_fd()) { // listenfd have pending connection
                    //sockaddr_in peeraddr; 
                    //socklen_t peeraddrsz;
                    //int commfd = accept4(listenfd, (sockaddr *)&peeraddr, &peeraddrsz, SOCK_NONBLOCK);
                    //if (commfd == -1) { perror("accept()"); exit(1); }
                    InetAddr clientaddr;
                    Socket *clientsock = new Socket(servsock.accept_nbconn(clientaddr));
                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",
                            clientsock->get_fd(), clientaddr.get_ip(), clientaddr.get_port());
                    epoll_add_event(epfd, EPOLLIN, clientsock->get_fd());
                } else {
                    char buffer[READBUFCAP];
                    while (true) {
                        memset(buffer, 0, sizeof(buffer));
                        int bytesred = recv(eventbuf[i].data.fd, buffer, READBUFCAP, 0);
                        if (bytesred == 0) { // client closes the socket
                            printf("client: fd=%d disconnected with 0 byte\n", eventbuf[i].data.fd);
                            close(eventbuf[i].data.fd); // automatically delete event in eventbuf
                            break;
                        } else if (bytesred == -1) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { break; }
                            perror("recv()");
                            exit(1);
                        } else {
                            printf("Received from fd=%d: %s\n", eventbuf[i].data.fd, buffer);
                            send(eventbuf[i].data.fd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            } 
            else if (eventbuf[i].events & EPOLLRDHUP) {
                printf("client: fd=%d disconnected with EPOLLRDHUP\n", eventbuf[i].data.fd);
                close(eventbuf[i].data.fd); // automatically delete event in eventbuf
            } 
            else if (eventbuf[i].events & EPOLLOUT) {}
            else {
                printf("client: fd=%d error with unknown event\n", eventbuf[i].data.fd);
                close(eventbuf[i].data.fd); // automatically delete event in eventbuf
            }
        }
    }
}