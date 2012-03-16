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

#define MAXLINE 1000

void echo(int lfd)
{
    char recvline[MAXLINE + 1];
    while (1)
    {
        sockaddr_in cliaddr;
        int cliaddrlen = sizeof(cliaddr);
        int connfd = accept(lfd, (sockaddr*)&cliaddr, (socklen_t*)&cliaddrlen);
        if (connfd < 0)
        {
            perror("accept");
            break;
        }

        while (1)
        { 
            int n = read(connfd, recvline, MAXLINE);
            if (n > 0)
            {
                recvline[n] = 0;
                if (write(connfd, recvline, strlen(recvline)) < 0)
                {
                    perror("write");
                    break;
                }
            }
            else if (n < 0)   
            {
                perror("read");
                break;
            }
            else // n == 0
            {
                cout << "read EOF" << endl;
                break;
            }
        }

        close(connfd);
    }
}

void mute(int lfd)
{
    //char recvline[MAXLINE + 1];
    while (1)
    {
        sockaddr_in cliaddr;
        int cliaddrlen = sizeof(cliaddr);
        int connfd = accept(lfd, (sockaddr*)&cliaddr, (socklen_t*)&cliaddrlen);
        if (connfd < 0)
        {
            perror("accept");
            break;
        }
    }
}


int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7777);

    if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))< 0)
    {
        perror("bind");
        return 0;
    }

    if (listen(listenfd, 1) < 0)
    {
        perror("listen");
        return 0;
    }

    //echo(listenfd);
    mute(listenfd);

    close(listenfd);  
    return 0;
}
