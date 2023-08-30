#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
#define SERVER_PORT 12345
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT); // htonl: host to nel=====> htons: host to network u_short!!!
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);
    if (connect(sock_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("connect error: %s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }
    printf("==== start to send data to server ====\n");
    std::string recv_msg;
    auto recv_buf = std::make_unique<char[]>(1024);
    while (getline(std::cin, recv_msg)) {
        auto len = send(sock_fd, recv_msg.c_str(), recv_msg.length(), 0);
        if (len < 0) {
            printf("==== send msg error: %s(errno: %d)\n", strerror(errno), errno);
            return 0;
        }
        std::cout << "==== [" << len << " bytes sent]\n";
        memset(recv_buf.get(), 0, 1024);
        len = recv(sock_fd, recv_buf.get(), 1024, 0);
        if (len < 0) {
            std::cout << "==== received data failed\n";
            return 0;
        }
        std::cout << "==== [" << len << " bytes received]:\n" << recv_buf.get() << "\n";
    }
    printf("==== exit ====\n");
    return 0;
}