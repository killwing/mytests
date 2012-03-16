#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <list>
#include <cstring>
#include <stdio.h>
#include <signal.h>
using namespace std;

#define MAXLINE 5000
#define MAXEPOLLSIZE 10000  

// readiness notification from the kernel is only a hint; 
// the file descriptor might not be ready anymore when you try to read from it. 
// That's why it's important to use nonblocking mode when using readiness notification. (or you may blocked!)
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

int invite(int epfd)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr("192.168.16.1");
    localaddr.sin_port = htons(5060);

    if (bind(fd, (sockaddr*)&localaddr, sizeof(localaddr))< 0)
    {
        perror("bind");
        return 0;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5061);
    if (inet_pton(AF_INET, "192.168.16.55", &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 0;
    }

    // auto bind local address & port
    if (connect(fd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        return 0;
    }

    setnonblocking(fd);
    // add
    epoll_event ev;  
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;  // Edge Triggered, the absent EPOLLET means Level Triggered
    ev.data.fd = fd;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0)  
    {  
        perror("epoll_ctl");
        return 0;  
    }  

    setsendbuff(fd, 10240);
    std::string str = "INVITE sip:704@192.168.16.55:5061 SIP/2.0\r\n\
Via: SIP/2.0/TCP 192.168.16.1:5061;branch=z9hG4bK123456\r\n\
Max-Forwards: 70\r\n\
From: seagull <sip:kelvin@192.168.16.1:5061>;tag=1\r\n\
To: sut <sip:704@192.168.16.55:5061>\r\n\
Call-ID: 1@192.168.16.1\r\n\
CSeq: 1 INVITE\r\n\
Contact: <sip:192.168.16.1:5061>\r\n\
Accept: application/sdp\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 116\r\n\
\r\n\
v=0\r\n\
o=- 0 0 IN IP4 192.168.16.1\r\n\
s=session\r\n\
c=IN IP4 192.168.16.1\r\n\
b=CT:1000\r\n\
t=0 0\r\n\
m=audio 64162 RTP/AVP 0 8 18\r\n";
                     
    int n = write(fd, str.c_str(), str.size());
    if (n < 0)
    {
        perror("write");
        return 0;
    }
    else
    {
        cout<<"wrote ok on invite fd:"<<fd<<endl;
    }

    return fd;
}

int main()  
{  
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE 

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("socket");
        return 0;
    }
    
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.16.1");
    servaddr.sin_port = htons(5061);

    if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))< 0)
    {
        perror("bind");
        return 0;
    }

    if (listen(listenfd, 10) < 0)
    {
        perror("listen");
        return 0;
    }


    epoll_event ev;  
    epoll_event events[MAXEPOLLSIZE];  

    int epfd = epoll_create(MAXEPOLLSIZE);  
    int curfds = 1;


    ev.events = EPOLLIN | EPOLLET;  
    ev.data.fd = listenfd;  
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)   
    {  
        perror("epoll_ctl");
        return 0;  
    } 

    int invitefd = invite(epfd);
    if (invitefd > 0)
    {
        curfds++;
    }
    else
    {
        return 0;
    }

    while (1)   
    {  
        cout<<"epoll waiting..."<<endl;
        int nfds = epoll_wait(epfd, events, curfds, -1);  // -1 means blocking
        if (nfds == -1)  
        {  
            perror("epoll_wait");  
            break;  
        }  

        for (int n = 0; n < nfds; ++n)  
        {  
            if (events[n].data.fd == listenfd)   
            {  
                int connfd = accept(listenfd, NULL, NULL);  
                if (connfd < 0)   
                {  
                    perror("accept");  
                    continue;  
                }   
                else
                {
                    cout << "new connection: " << connfd << endl;
                }
         
                setnonblocking(connfd);
                ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT | EPOLLET;  // Edge Triggered, the absent EPOLLET means Level Triggered
                ev.data.fd = connfd;  
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) < 0)  
                {  
                    perror("epoll_ctl");
                    continue;  
                }  
                curfds++;  
            }   
            else
            {
                char recvline[MAXLINE + 1] = { 0 };
                bool closefd = false;

                if (events[n].events & EPOLLERR)
                {
                    cout << "error happened on "<< events[n].data.fd << endl;
                    closefd = true;
                }
                else if (events[n].events & EPOLLHUP)
                {
                    cout << "hup happened on "<< events[n].data.fd << endl;
                    closefd = true;
                }
                else
                {
                    if (events[n].events & EPOLLIN)
                    {  
                        int fd = events[n].data.fd;
                        cout << "ready to read on "<< fd << endl;

                        int len = read(fd, recvline, MAXLINE);
                        if (len > 0)
                        {
                            recvline[len] = 0; // set end str
                            cout<<"recved: "<<endl;
                            cout<<recvline<<endl;
                        }
                        else if (len < 0)   
                        {
                            perror("read");
                            if (errno == EAGAIN)
                            {
                                cout<<"eagian!"<<endl;
                                continue;
                            }
                            else
                            {
                                closefd = true;
                            }
                        }
                        else // len == 0
                        {
                            cout << "read EOF" << endl;
                            closefd = true;
                        }
                    }  

                    if (events[n].events & EPOLLOUT)
                    {
                        cout << "ready to write on "<< events[n].data.fd << endl;

                        if (!closefd && strlen(recvline) > 0)
                        {
                            if (write(events[n].data.fd, recvline, strlen(recvline)) < 0)
                            {
                                perror("write");
                                if (errno == EAGAIN)
                                {
                                    cout<<"eagian!"<<endl;
                                    continue;
                                }
                                else
                                {
                                    closefd = true;
                                }
                            }
                            else
                            {
                                cout<<"wrote ok"<<endl;
                            }
                        }
                    }
                }

                if (closefd)  
                {  
                    cout << "removed connection: " << events[n].data.fd << endl;
                    close(events[n].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);  
                    curfds--;  
                }  
            }  
        }
    }  

    close(listenfd);  
    return 0;  
}  
