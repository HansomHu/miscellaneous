#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/socket.h>
#include <netinet/in.h> // for sockaddr_in
#include <netinet/tcp.h> // for TCP_KEEPIDLE
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    int sfd, portno, n;

    struct sockaddr_in srvaddr;
    struct hostent* host;

    if (argc < 3) {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("ERROR: socket()");
        exit(0);
    }

    int flags = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&flags, sizeof(flags))) {
        perror("ERROR: setsocketopt(), SO_KEEPALIVE");
        exit(0);
    };

    flags = 10;
    if (setsockopt(sfd, SOL_TCP, TCP_KEEPIDLE, (void*)&flags, sizeof(flags))) {
        perror("ERROR: setsocketopt(), SO_KEEPIDLE");
        exit(0);
    };

    host = gethostbyname(argv[1]);
    if (host == NULL) {
        fprintf(stderr, "ERROR: host does not exist");
        exit(0);
    }

    bzero((char*)&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(portno);

    bcopy((char*)host->h_addr, (char*)&srvaddr.sin_addr.s_addr, host->h_length);

    if (connect(sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
        perror("ERROR: connect()");
        exit(0);
    }

    while (true) {
        std::string content = "12345";
        if (send(sfd, content.c_str(), content.size() + 1, 0) < 0) {
            std::cout << "send failed\n";
        }

        char recvBuf[20] = {0};
        if (recv(sfd, recvBuf, sizeof(recvBuf), 0) < 0) {
            std::cout << "recv failed\n";
        } else {
            std::cout << "Received data: " << recvBuf << "\n";
        }
        usleep(100000);
    }

    return 0;
}
