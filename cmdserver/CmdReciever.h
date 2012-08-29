#ifndef CMDRECIEVER_H
#define CMDRECIEVER_H

#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>
#include "MutexLock.h"

class EpInfo;
class CmdServer;

// a tcp server using epoll
class CmdReciever {
public:
    CmdReciever(CmdServer* server, const std::string& addr, unsigned short port);
    ~CmdReciever();

    void start();
    void stop();
    void recieve();

private:
    void buildCmds(char* p, int len);
    void handleData(char* p, int len);

    pthread_t recvThrd_;
    bool running_;

    boost::scoped_ptr<EpInfo> epInfo_;

    typedef std::vector<std::string> CmdQueue;
    CmdQueue cmdQueue_;
    MutexLock queueLock_;

    CmdServer* server_;
};

#endif // CMDRECIEVER_H

