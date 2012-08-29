#include "CmdServer.h"
#include "CmdReciever.h"
#include "CmdDispatcher.h"

CmdServer::CmdServer()
    : reciever_(new CmdReciever(this, "127.0.0.1", 30000)) {
}

CmdServer::~CmdServer() {
}

void
CmdServer::start() {
    dispatcher_->start();
    reciever_->start();
}

void
CmdServer::stop() {
    reciever_->stop();
    dispatcher_->stop();
}


/*
#include <stdio.h>
void 
CmdServer::test() {
    char cmd1[3];
    cmd1[0] = 'x';
    cmd1[1] = 'y';
    cmd1[2] = 'z';
    buildCmds(cmd1, sizeof(cmd1));

    cmd1[0] = 'x';
    cmd1[1] = '\0';
    cmd1[2] = 'z';
    buildCmds(cmd1, sizeof(cmd1));

    char cmds[] = "abc\0def\0ghi";
    buildCmds(cmds, sizeof(cmds));

    cmd1[0] = 'x';
    cmd1[1] = '\0';
    cmd1[2] = 'z';
    buildCmds(cmd1, sizeof(cmd1));

    cmd1[0] = 'y';
    cmd1[1] = 'x';
    cmd1[2] = '\0';
    buildCmds(cmd1, sizeof(cmd1));

    CmdQueue::iterator it = cmdQueue_.begin();
    for (; it != cmdQueue_.end(); ++it) {
        printf("%s\n", it->c_str());
    }
}
*/
