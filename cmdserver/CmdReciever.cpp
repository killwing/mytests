#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <stdexcept>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "CmdReciever.h"
#include "CmdServer.h"
#include "Condition.h"

namespace {

const unsigned int MAXEPOLLSIZE = 5000;
const unsigned int MAXBUFSIZE = 500;

// readiness notification from the kernel is only a hint;
// the file descriptor might not be ready anymore when you try to read from it.
// That's why it's important to use nonblocking mode when using readiness notification. (or you may blocked!)
bool setNonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0) {
        perror("fcntl");
        return false;
    }
    return true;
}

void* recieveProc(void* arg) {
    CmdReciever* reciever = (CmdReciever*)arg;
    reciever->recieve();
    return 0;
}

}

struct EpInfo {
    unsigned int curfds;
    int listenfd;
    int fd;
    epoll_event events[MAXEPOLLSIZE];
};

CmdReciever::CmdReciever(CmdServer* server, const std::string& addr, unsigned short port)
    : server_(server), running_(false), epInfo_(new EpInfo) {

    epInfo_->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (epInfo_->listenfd < 0) {
        perror("socket");
        throw std::runtime_error(strerror(errno));
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    servaddr.sin_port = htons(port);

    if (bind(epInfo_->listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        throw std::runtime_error(strerror(errno));
    }

    // max 10 pending connections
    if (listen(epInfo_->listenfd, 10) < 0) {
        perror("listen");
        throw std::runtime_error(strerror(errno));
    }

    epInfo_->fd = epoll_create(MAXEPOLLSIZE);
    epInfo_->curfds = 1;

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = epInfo_->listenfd;
    if (epoll_ctl(epInfo_->fd, EPOLL_CTL_ADD, epInfo_->listenfd, &ev) < 0) {
        perror("epoll_ctl");
        throw std::runtime_error(strerror(errno));
    }
}

CmdReciever::~CmdReciever() {
    pthread_join(recvThrd_, 0);
    close(epInfo_->listenfd);
}

void
CmdReciever::start() {
    if (pthread_create(&recvThrd_, 0, &recieveProc, this)) {
        throw std::runtime_error("pthread_create: can not start recieving thread.");
    }
}

void
CmdReciever::stop() {
    running_ = false;
}

void 
CmdReciever::buildCmds(char* p, int len) {
    if (!p || !len) {
        return;
    }

    static std::string cmdBuf;
    int i = 0;
    for (; i < len; ++i) {
        if (p[i] == 0) {
            break;
        }
    }

    if (i < len) {
        {
            LockGuard lock(queueLock_);
            cmdQueue_.push_back(cmdBuf + std::string(p, i));
            //server_->onDataNotify();
        }
        cmdBuf.clear();
        buildCmds(&p[i+1], len-i-1);
    } else { // not completed cmd
        cmdBuf += std::string(p, i);
    }
}

void
CmdReciever::handleData(char* p, int len) {
    buildCmds(p, len);
}

void
CmdReciever::recieve() {
    running_ = true;

    epoll_event* events = epInfo_->events;
    while (running_) {

        printf("epoll waiting... fds: %d\n", epInfo_->curfds);
        int nfds = epoll_wait(epInfo_->fd, events, epInfo_->curfds, -1);  // -1 means blocking
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == epInfo_->listenfd) {
                int connfd = accept(epInfo_->listenfd, NULL, NULL);
                if (connfd < 0) {
                    perror("accept");
                    continue;
                } else {
                    printf("new connection: %d\n", connfd);
                }

                setNonblocking(connfd);
                epoll_event ev;
                ev.events = EPOLLIN | EPOLLERR | EPOLLHUP; // | EPOLLET;  // Edge Triggered
                ev.data.fd = connfd;
                if (epoll_ctl(epInfo_->fd, EPOLL_CTL_ADD, connfd, &ev) < 0) {
                    perror("epoll_ctl");
                    continue;
                }
                epInfo_->curfds++;
            } else {
                bool closefd = true;
                int fd = events[n].data.fd;
                if (events[n].events & EPOLLERR) {
                    printf("error happened on fd: %d\n", fd);
                } else if (events[n].events & EPOLLHUP) {
                    printf("hup happened on fd: %d\n", fd);
                } else if (events[n].events & EPOLLIN) {
                    char buf[MAXBUFSIZE] = { 0 };
                    int len = read(fd, buf, MAXBUFSIZE);
                    if (len > 0) {
                        //printf("recieved data on fd: %d, %s\n", fd, buf);
                        handleData(buf, len); 
                        closefd = false;
                    } else if (len < 0) {
                        perror("read");
                        if (errno == EAGAIN) {
                            printf("eagain on fd: %d try again\n", fd);
                            continue;
                        }
                    } else { // len == 0
                        printf("recieved EOF on fd: %d\n", fd);
                    }
                }

                if (closefd) {
                    printf("removed connection: %d\n", fd);
                    epoll_ctl(epInfo_->fd, EPOLL_CTL_DEL, fd, &events[n]);
                    close(fd);
                    epInfo_->curfds--;
                }
            }
        }
    }

    running_ = false;
}


