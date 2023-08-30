#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // for sockaddr_in
#include <unistd.h>

#define BUF_SIZE 256

int main(int argc, char** argv) {
    int sfd, rfd, portno, clilen;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        perror("ERROR: no port\n");
        exit(1);
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("ERROR: socket()");
        exit(1);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: bind()");
        exit(1);
    }

    listen(sfd, 5);

    clilen = sizeof(cli_addr);
    rfd = accept(sfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
    if (rfd < 0) {
        perror("ERROR: accept()");
        exit(1);
    }

    while (1) {
        n = read(rfd, buffer, BUF_SIZE);
        if (n <= 0) {
            printf("read() ends\n");
            break;
        }

        printf("received: %s %d\n", buffer, n);
        snprintf(buffer, BUF_SIZE, "got it");
        n = write(rfd, buffer, sizeof("got it"));
        if (n < 0) {
            perror("ERROR: write()");
            exit(1);
        }
    }

    return 0;
}