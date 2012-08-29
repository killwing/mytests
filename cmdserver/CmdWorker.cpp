#include <stdio.h>
#include <syscall.h>
#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "CmdWorker.h"
#include "CmdBuffer.h"

namespace {

void* workProc(void* arg) {
    CmdWorker* worker = (CmdWorker*)arg;
    worker->run();
    return 0;
}

}


CmdWorker::CmdWorker(char type, CmdBuffer* buffer)
    : type_(type), buffer_(buffer) {
    if (pthread_create(&workThrd_, 0, &workProc, this)) {
        throw std::runtime_error("pthread_create: can not start work thread.");
    }
}

CmdWorker::~CmdWorker() {
    pthread_join(workThrd_, 0);
}


void
CmdWorker::run() {
    printf("worker type[%d] thread[%ld] start\n", type_, syscall(SYS_gettid));
    while (true) {

        Cmds cmds;
        {
            LockGuard g(buffer_->getLock());
            while (buffer_->empty(type_)) {
                printf("worker type[%d] thread[%ld] waiting...\n",
                    type_, syscall(SYS_gettid));
                buffer_->wait();
            }

            // copy out
            cmds = buffer_->popCmds(type_);
        }


        // process all cmds
        printf("ready to execute %d cmds[%d] on thread[%ld]\n", 
            cmds.size(), type_, syscall(SYS_gettid));
        Cmds::iterator it = cmds.begin();
        for (; it != cmds.end(); ++it) {
            printf("execute cmd[%d:%s] on thread[%ld]\n", 
                type_, it->c_str(), syscall(SYS_gettid));

            if (system(it->c_str()) < 0) {
                printf("failed to execute cmd[%s]\n", it->c_str());
            }
        }
    }
}

