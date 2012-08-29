#ifndef CMDDISPATHER_H
#define CMDDISPATHER_H

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <pthread.h>

class Command;

class CmdDispatcher {
public:
    CmdDispatcher();
    ~CmdDispatcher();

    void start();
    void dispatch();
    void stop();
    void clearCmdTable(char type);

private:
    typedef std::map<char, boost::shared_ptr<Command> > CmdTable;
    CmdTable cmds_;

    pthread_t dispThrd_;
    bool running_;
};

#endif // CMDDISPATHER_H

