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
#include "Channel.h"

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
    Channel *servchannel = new Channel(&ep, servsock.get_fd(), true);
    servchannel->enable_reading();
    
    while (true) {
        //std::vector<epoll_event> events;
        std::vector<Channel *> channels = ep.loop();
        for (auto &ch : channels) 
        {
            ch->handle_event(&servsock);
            /*
            if (ch->get_revents() & (EPOLLIN | EPOLLPRI)) {
                if (ch == servchannel) { // listenfd have pending connection
                    InetAddr clientaddr;
                    Socket *clientsock = new Socket(servsock.accept_nbconn(clientaddr));
                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",
                            clientsock->get_fd(), clientaddr.get_ip(), clientaddr.get_port());
                    Channel *clichannel = new Channel(&ep, clientsock->get_fd(), false);
                    clichannel->use_ET();
                    clichannel->enable_reading();
                    
                } else {
                    char buffer[CHAN_READBUFCAP];
                    while (true) {
                        memset(buffer, 0, sizeof(buffer));
                        int bytesred = recv(ch->get_fd(), buffer, CHAN_READBUFCAP, 0);
                        if (bytesred == 0) { // client closes the socket
                            printf("client: fd=%d disconnected with 0 byte\n", ch->get_fd());
                            close(ch->get_fd()); // automatically delete event in eventbuf
                            break;
                        } else if (bytesred == -1) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { break; }
                            perror("recv()");
                            exit(1);
                        } else {
                            printf("Received from fd=%d: %s\n", ch->get_fd(), buffer);
                            send(ch->get_fd(), buffer, strlen(buffer), 0);
                        }
                    }
                }
            } 
            else if (ch->get_revents() & EPOLLRDHUP) {
                printf("client: fd=%d disconnected with EPOLLRDHUP\n", ch->get_fd());
                close(ch->get_fd()); // automatically delete event in eventbuf
            } 
            else if (ch->get_revents() & EPOLLOUT) {}
            else {
                printf("client: fd=%d error with unknown event\n", ch->get_fd());
                close(ch->get_fd()); // automatically delete event in eventbuf
            }
            */
        }
    }
}