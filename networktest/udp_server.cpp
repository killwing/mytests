#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <stdio.h>
using namespace std;

#define MAXLINE 10

void echo(int fd)
{
    char recvline[MAXLINE + 1];
    while (1)
    { 
        sockaddr_in cliaddr;
        int cliaddrlen = sizeof(cliaddr);
        int n = recvfrom(fd, recvline, MAXLINE, 0, (sockaddr*)&cliaddr, (socklen_t*)&cliaddrlen);
        if (n > 0)
        {
            recvline[n] = 0;
            if (sendto(fd, recvline, strlen(recvline), 0, (sockaddr*)&cliaddr, cliaddrlen) < 0)
            {
                perror("sendto");
                break;
            }
        }
        else if (n < 0)   
        {
            perror("recvfrom");
            break;
        }
        else // n == 0
        {
            // seems remote shutdown can not take us here, so.. should set timeout of recvfrom()
            break;
        }
    }
}

void mute()
{
    sleep(600);
}

void eat(int fd)
{
    char recvline[MAXLINE + 1];
    while (1)
    { 
        sockaddr_in cliaddr;
        int cliaddrlen = sizeof(cliaddr);
        int n = recvfrom(fd, recvline, MAXLINE, 0, (sockaddr*)&cliaddr, (socklen_t*)&cliaddrlen);
        if (n > 0)
        {
            recvline[n] = 0;
            cout<<"recvd "<<n<<": "<<recvline<<endl;
        }
        else if (n < 0)   
        {
            perror("recvfrom");
            break;
        }
        else // n == 0
        {
            // seems remote shutdown can not take us here, so.. should set timeout of recvfrom()
            break;
        }
    }
}


int main()
{
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7777);

    if (bind(socketfd, (sockaddr*)&servaddr, sizeof(servaddr))< 0)
    {
        perror("bind");
        return 0;
    }

    //echo(socketfd)
    //mute();
    eat(socketfd);

    close(socketfd);
    return 0;
}
