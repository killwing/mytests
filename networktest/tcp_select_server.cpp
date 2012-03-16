#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <cstring>
#include <stdio.h>
using namespace std;

#define MAXLINE 1000


// readiness notification from the kernel is only a hint; 
// the file descriptor might not be ready anymore when you try to read from it. 
// That's why it's important to use nonblocking mode when using readiness notification.
void setnonblocking(int sockfd)  
{  
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)
    {
        perror("fcntl");
    }
}  

int main()   
{   
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
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

    // the max waiting num of connection is 10
    if (listen(listenfd, 10) < 0)
    {
        perror("listen");
        return 0;
    }

    list<int> connfds;
    while (1)   
    {   
        // after being called, select() will clear no ready fds with the fd_sets, so reset all of them everytime. 
        fd_set rd, wr, ex;
        FD_ZERO(&rd);   
        FD_ZERO(&wr);   
        FD_ZERO(&ex);   
        FD_SET(listenfd, &rd);   
        for (list<int>::iterator it = connfds.begin(); it != connfds.end(); ++it)
        {
            FD_SET(*it, &rd);
            FD_SET(*it, &wr);
            FD_SET(*it, &ex);
        }

        // timeout
        timeval tv; 
        tv.tv_sec = 10; 
        tv.tv_usec = 0;

        // max fd plus 1
        int maxfdp1;
        if (connfds.empty())
        {
            maxfdp1 = listenfd + 1;
        }
        else
        {
            maxfdp1 = *max_element(connfds.begin(), connfds.end()) + 1;
        }

        cout<<"select waiting..."<<endl;
        int ret = select(maxfdp1, &rd, NULL, &ex, &tv);  // do not return when ready to write
        if (ret < 0)   
        {   
            perror("select");
            break;   
        }
        else if (ret == 0)
        {
            cout << "timeout, no one is ready" << endl;
            continue;
        }
        else
        {
            if (FD_ISSET(listenfd, &rd))   
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
                    setnonblocking(connfd);
                    connfds.push_back(connfd); 
                }   
            }   
            else   
            {   
                for (list<int>::iterator it = connfds.begin(); it != connfds.end(); ++it)
                {   
                    // set buffer for one fd
                    char recvline[MAXLINE + 1] = {0};
                    bool closefd = false;

                    if (FD_ISSET(*it, &rd))   
                    {   
                        cout << "ready to read on " << *it << endl;
                        int n = read(*it, recvline, MAXLINE);
                        if (n > 0)
                        {
                            recvline[n] = 0; // set end str
                        }
                        else if (n < 0)   
                        {
                            perror("read");
                            if (errno == EAGAIN)
                            {
                                continue;
                            }
                            else
                            {
                                closefd = true;
                            }
                        }
                        else // n == 0
                        {
                            cout << "read EOF" << endl;
                            closefd = true;
                        }
                    }   
                    
                    if (FD_ISSET(*it, &wr))
                    {
                        cout << "ready to write on " << *it << endl;
                        if (!closefd && strlen(recvline) > 0)
                        {
                            if (write(*it, recvline, strlen(recvline)) < 0)
                            {
                                perror("write");
                                if (errno == EAGAIN)
                                {
                                    continue;
                                }
                                else
                                {
                                    closefd = true;
                                }
                            }
                        }
                    }

                    if (FD_ISSET(*it, &ex))
                    {
                        cout << "exception occurred on " << *it << endl;
                        closefd = true;
                    }

                    if (closefd)
                    {
                        close(*it);

                        cout << "removed connection: " << *it << endl;
                        connfds.remove(*it);
                        break; // the list is invalid, skip this round
                    }
                }   
            }   
        }
    }         

    close(listenfd);  
    return 0;  
}   

