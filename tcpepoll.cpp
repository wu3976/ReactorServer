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
#include "Epoll.h"

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

int main(int argc, char **argv) {
    string ip;
    unsigned short port = parsecmd(argc, argv, ip);
    Socket servsock(create_nonblocking_socket());
    InetAddr servaddr(ip, port);
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind_addr(servaddr);
    servsock.listen_conn();
    printf("listening connections at %s:%d\n", 
            servaddr.get_ip(), servaddr.get_port());
    Epoll ep;
    ep.add_fdevent(servsock.get_fd(), EPOLLIN); 
    std::vector<epoll_event> events;
    while (true) {
        events = ep.loop();
        for (auto &evt : events) 
        {
            if (evt.events & (EPOLLIN | EPOLLPRI)) {
                if (evt.data.fd == servsock.get_fd()) { // listenfd have pending connection
                    InetAddr clientaddr;
                    Socket *clientsock = new Socket(servsock.accept_nbconn(clientaddr));
                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",
                            clientsock->get_fd(), clientaddr.get_ip(), clientaddr.get_port());
                    ep.add_fdevent(clientsock->get_fd(), EPOLLIN | EPOLLET);
                } else {
                    char buffer[READBUFCAP];
                    while (true) {
                        memset(buffer, 0, sizeof(buffer));
                        int bytesred = recv(evt.data.fd, buffer, READBUFCAP, 0);
                        if (bytesred == 0) { // client closes the socket
                            printf("client: fd=%d disconnected with 0 byte\n", evt.data.fd);
                            close(evt.data.fd); // automatically delete event in eventbuf
                            break;
                        } else if (bytesred == -1) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { break; }
                            perror("recv()");
                            exit(1);
                        } else {
                            printf("Received from fd=%d: %s\n", evt.data.fd, buffer);
                            send(evt.data.fd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            } 
            else if (evt.events & EPOLLRDHUP) {
                printf("client: fd=%d disconnected with EPOLLRDHUP\n", evt.data.fd);
                close(evt.data.fd); // automatically delete event in eventbuf
            } 
            else if (evt.events & EPOLLOUT) {}
            else {
                printf("client: fd=%d error with unknown event\n", evt.data.fd);
                close(evt.data.fd); // automatically delete event in eventbuf
            }
        }
    }
}