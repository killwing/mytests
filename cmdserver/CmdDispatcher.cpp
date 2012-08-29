#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "CmdDispatcher.h"
#include "Command.h"
#include "stdio.h"

namespace {

void* dispatchProc(void* arg) {
    CmdDispatcher* dispatcher = (CmdDispatcher*)arg;
    dispatcher->dispatch();
    return 0;
}

void* executionProc(void* arg) {
    Command* cmd = (Command*)arg;
    cmd->execute();
    return 0;
}

}


CmdDispatcher::CmdDispatcher() : running_(false) {
}

CmdDispatcher::~CmdDispatcher() {
    pthread_join(dispThrd_, 0);
}

void
CmdDispatcher::start() {
    if (pthread_create(&dispThrd_, 0, &dispatchProc, this)) {
        throw std::runtime_error("pthread_create: can not start dispatching thread.");
    }
}

void
CmdDispatcher::stop() {
    running_ = false;
}

void
CmdDispatcher::dispatch() {
    running_ = true;
    while (running_) {
        //pthread_cond_wait();

        char type;
        std::string cmdstr;
        CommandPtr cmd = boost::make_shared<Command>(type, cmdstr, boost::bind(&CmdDispatcher::clearCmdTable, this, _1));

        CmdTable::iterator it = cmds_.find(type);
        if (it != cmds_.end()) { // new type
            cmds_.insert(CmdTable::value_type(type, cmd));

            pthread_t thrd;
            if (pthread_create(&thrd, 0, &executionProc, cmd.get()), 0) {
                printf("pthread_create: can not start execution thread.");
            }
            pthread_detach(thrd); // need not join, make detachable

        } else {
            it->second->next(cmd);
            it->second = cmd; // save last one
        }
    }

    running_ = false;
}

void
CmdDispatcher::clearCmdTable(char type) {
    cmds_.erase(type);
}
