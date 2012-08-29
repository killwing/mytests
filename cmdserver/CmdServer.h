#ifndef CMDSERVER_H
#define CMDSERVER_H

#include <boost/smart_ptr.hpp>
#include "MutexLock.h"

class CmdReciever;
class CmdDispatcher;

class CmdServer {
public:
    CmdServer();
    ~CmdServer();

    void start();
    void stop();

private:

    boost::scoped_ptr<CmdReciever> reciever_;
    boost::scoped_ptr<CmdDispatcher> dispatcher_;
};


#endif // CMDSERVER_H

