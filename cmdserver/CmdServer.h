#ifndef CMDSERVER_H
#define CMDSERVER_H

#include <string>
#include <boost/smart_ptr.hpp>

class EpInfo;
class CmdDispatcher;

// a tcp server using epoll
class CmdServer {
public:
    CmdServer(const std::string& addr, unsigned short port);
    ~CmdServer();

    void start();
    void stop();
    void recieve();

private:
    void buildCmds(char* p, int len);
    void handleData(char* p, int len);

    pthread_t recvThrd_;
    bool running_;

    boost::scoped_ptr<EpInfo> epInfo_;
    boost::scoped_ptr<CmdDispatcher> dispatcher_;
};

#endif // CMDSERVER_H

