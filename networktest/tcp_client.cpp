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


void echo(int fd)
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

        if (write(fd, str.c_str(), str.size()) < 0)
        {
            perror("write");
            break;
        }

        int n = read(fd, recvline, MAXLINE);
        if (n > 0)
        {
            recvline[n] = 0;
            cout << recvline << endl;
        }
        else if (n < 0)   
        {
            perror("read");
            break;
        }
        else
        {
            cout << "read EOF" << endl;
            break;
        }
    }
}

void verbose(int fd)
{
    setnonblocking(fd);
    setsendbuff(fd, 10240);
    //std::string str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789000";
    std::string str = "a";
    int sum = 0;
    while (1)
    {
        int n = write(fd, str.c_str(), str.size());
        if (n < 0)
        {
            perror("write");
            cout<<"errno: "<<errno<<", totally write: "<<sum<<endl;
            break;
        }
        else
        {
            sum += n;
            cout<<"wrote: "<<sum<<endl;
        }
    }
}

int main()
{ 
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE 

    int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7777);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 0;
    }

    // auto bind local address & port
    if (connect(socketfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        return 0;
    }

    echo(socketfd);
    //verbose(socketfd);
    //sleep(1000);


    close(socketfd);
    cout<<"exit"<<endl;
    return 0;
}
