#ifndef CMDDISPATCHER_H
#define CMDDISPATCHER_H

#include <map>
#include <boost/smart_ptr.hpp>
#include "MutexLock.h"
#include "Condition.h"

class CmdChannel;

class CmdDispatcher {
public:
    void dispatch(char type, const std::string& cmd);

    void notify() {
        cond_.notify();
    }

    MutexLock& getLock() {
        return lock_;
    }

private:
    MutexLock lock_;
    Condition cond_;

    typedef std::map<char, boost::shared_ptr<CmdChannel> > CmdChannels;
    CmdChannels channels_;
};

#endif // CMDDISPATCHER_H

