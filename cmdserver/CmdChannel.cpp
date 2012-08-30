#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <stdexcept>
#include "MutexLock.h"
#include "CmdChannel.h"
#include "Condition.h"

namespace {

void* workProc(void* arg) {
    CmdChannel* ch = (CmdChannel*)arg;
    ch->run();
    return 0;
}

}


CmdChannel::CmdChannel(MutexLock& lock, Condition& cond)
    : lock_(lock), cond_(cond) {
    if (pthread_create(&workThrd_, 0, &workProc, this)) {
        throw std::runtime_error("pthread_create: can not start work thread.");
    }
}

CmdChannel::~CmdChannel() {
    pthread_join(workThrd_, 0);
}


void
CmdChannel::run() {
    printf("worker thread[%ld] start\n", syscall(SYS_gettid));
    while (true) {

        Cmds cmds;
        {
            LockGuard g(lock_);
            while (cmds_.empty()) {
                printf("worker thread[%ld] waiting...\n", syscall(SYS_gettid));
                cond_.wait(lock_);
            }

            // copy out
            cmds = cmds_;
            cmds_.clear();
        }


        // process all cmds
        printf("ready to execute %d cmds on thread[%ld]\n", cmds.size(), syscall(SYS_gettid));
        Cmds::iterator it = cmds.begin();
        for (; it != cmds.end(); ++it) {
            printf("execute cmd[%s] on thread[%ld]\n", it->c_str(), syscall(SYS_gettid));

            if (system(it->c_str()) < 0) {
                printf("failed to execute cmd[%s]\n", it->c_str());
            }
        }
    }
}

