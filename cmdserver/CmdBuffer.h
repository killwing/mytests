#ifndef CMDBUFFER_H
#define CMDBUFFER_H

#include <string>
#include <vector>
#include <map>
#include <pthread.h>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include "MutexLock.h"
#include "CmdWorker.h"

class Condition : public boost::noncopyable {
public:
    Condition() {
        pthread_cond_init(&cond_, 0);
    }

    ~Condition() {
        pthread_cond_destroy(&cond_);
    }

    void wait(MutexLock& mutex) {
        pthread_cond_wait(&cond_, mutex.get());
    }

    void notify() {
        pthread_cond_broadcast(&cond_);
    }
private:
    pthread_cond_t cond_;
};

typedef std::vector<std::string> Cmds;

class CmdBuffer {
public:
    void add(char type, const std::string& cmd) {
        CmdTable::iterator it = cmdTable_.find(type);
        if (it == cmdTable_.end()) { // new type
            Cmds& cmds = cmdTable_[type];
            cmds.push_back(cmd);
            workers_.push_back(boost::make_shared<CmdWorker>(type, this));
        } else {
            it->second.push_back(cmd);
        }
    }

    Cmds popCmds(char type) {
        Cmds& cmds = cmdTable_[type];
        Cmds result = cmds;
        cmds.clear();
        return result;
    }

    bool empty(char type) {
        return cmdTable_[type].empty();
    }

    void wait() {
        cond_.wait(lock_);
    }

    void notify() {
        cond_.notify();
    }

    MutexLock& getLock() {
        return lock_;
    }
private:
    MutexLock lock_;
    Condition cond_;
    typedef std::map<char, Cmds> CmdTable;
    typedef std::vector<boost::shared_ptr<CmdWorker> > CmdWorkers;
    CmdTable cmdTable_;
    CmdWorkers workers_;
};

#endif // CMDBUFFER_H

