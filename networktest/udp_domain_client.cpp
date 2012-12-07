#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#define MAXLINE 1000

void setsendbuff(int fd, socklen_t n = 0)
{
    socklen_t sendbuflen = 0;  
    socklen_t len = sizeof(sendbuflen);  
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void*)&sendbuflen, &len);  
    cout<<"before - sendbuf: "<<sendbuflen<<endl;  

    sendbuflen = n;  
    if (n > 0)
    {
        setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void*)&sendbuflen, len);  
    }
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void*)&sendbuflen, &len);  
    cout<<"after - sendbuf: "<<sendbuflen<<endl;  
}

void echo(int fd, const sockaddr_un& servaddr)
{
    char recvline[MAXLINE + 1];
    while (1)
    {
        string str;
        cin >> str;
        if (!cin.good())
        {
            cout << "I/O: input end or error" << endl;
            break;
        }

        if (sendto(fd, str.c_str(), str.size(), 0, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto");
            break;
        }

        int n = recvfrom(fd, recvline, MAXLINE, 0, 0, 0);
        if (n > 0)
        {
            recvline[n] = 0;
            cout << recvline << endl;
        }
        else if (n < 0)   
        {
            perror("recvfrom");
            break;
        }
        else // n == 0
        {
            break;
        }
    }
}

void verbose(int fd, const sockaddr_un& servaddr)
{
    //setsendbuff(fd);
    std::string str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789000";
    int sum = 0;
    while (1)
    {
        int n = sendto(fd, str.c_str(), str.size(), 0 /* MSG_DONTWAIT */, (sockaddr*)&servaddr, sizeof(servaddr));
        if ( n < 0)
        {
            perror("sendto");
            cout<<"errno: "<<errno<<", totally send: "<<sum<<endl;
            break;
        }
        else
        {
            sum += n;
            cout<<"totally send: "<<sum<<endl;
        }
    }
}

int main()
{ 
    int socketfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_un cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sun_family = AF_LOCAL;

    char temp[L_tmpnam];
    tmpnam(temp);
    printf("use temp path: %s\n", temp);
    strcpy(cliaddr.sun_path, temp);
    if (bind(socketfd, (sockaddr*)&cliaddr, sizeof(cliaddr))< 0)
    {
        perror("bind");
        return 0;
    }

    sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, "server.sock");

    //echo(socketfd, servaddr);
    verbose(socketfd, servaddr);

    close(socketfd);
    return 0;
}
