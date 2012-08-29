#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
using namespace std;

#define MAXLINE 1000

void setnonblocking(int sockfd)  
{  
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)
    {
        perror("fcntl");
    }
}

void sendCmd(int fd)
{
    int cnt = 1;
    while (cnt--) {
        std::string cmd("0date\0", 6);
        int n = write(fd, cmd.c_str(), cmd.size());
        if (n < 0) {
            perror("write");
        } else {
            cout<<"sent: "<<cmd<<endl;
        }
    }
}

int main() { 
    int cnt = 10;
    while (cnt--) {
        int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socketfd < 0) {
            perror("socket");
            break;
        }

        sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(30000);
        if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
        {
            perror("inet_pton");
            break;
        }

        // auto bind local address & port
        if (connect(socketfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            perror("connect");
            break;
        }

        sendCmd(socketfd);
        close(socketfd);
        sleep(1);
    }

    printf("exit\n");
    return 0;
}
