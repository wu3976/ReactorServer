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
#include "Eventloop.h"

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
    // Epoll ep;
    Eventloop evlp;
    Channel *servchannel = new Channel(evlp.get_ep(), servsock.get_fd());
    servchannel->set_readcallback(std::bind(&Channel::newconnection, 
            servchannel, &servsock));
    servchannel->enable_reading();
    evlp.run();
}