#ifndef CMDCHANNEL_H
#define CMDCHANNEL_H

#include <pthread.h>
#include <vector>
#include <string>

class MutexLock;
class Condition;

class CmdChannel {
public:
    CmdChannel(MutexLock& lock, Condition& cond);
    ~CmdChannel();

    void run();

    void push(const std::string& s) {
        cmds_.push_back(s);
    }
private:
    pthread_t workThrd_;

    typedef std::vector<std::string> Cmds;
    Cmds cmds_;

    MutexLock& lock_;
    Condition& cond_;
};

#endif // CMDCHANNEL_H

