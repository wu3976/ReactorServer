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
#include <netdb.h>

#define BUFCAP 1024

using namespace std;

int create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket()");
        exit(1);
    }
    return fd;
}

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

void connect_socket(int fd, sockaddr_in *psin) {
    if (connect(fd, (sockaddr *)psin, sizeof(*psin)) == -1) {
        perror("connect");
        exit(1);
    }
}

int main(int argc, char **argv) {
    string ipout;
    unsigned short port = parsecmd(argc, argv, ipout);
    int fd = create_socket();
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ipout.c_str());
    connect_socket(fd, &sin);
    char buf[BUFCAP];
    while(true) {
        memset(buf, 0, sizeof(buf));
        printf("Enter input: ");
        scanf("%s", buf);
        if (send(fd, buf, strlen(buf), 0) == -1) {
            perror("send()");
            exit(1);
        }
        memset(buf, 0, sizeof(buf));
        int numrecv = recv(fd, buf, sizeof(buf), 0);
        if (numrecv == -1) {
            perror("recv()");
            exit(1);
        }
        printf("Received: %s\n", buf);
    }
}