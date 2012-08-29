#ifndef CONDITION_H
#define CONDITION_H

#include <pthread.h>
#include <boost/noncopyable.hpp>
#include "MutexLock.h"

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
        pthread_cond_signal(&cond_);
    }
private:
    pthread_cond_t cond_;
};

struct Sync {
    MutexLock lock;
    Condition cond;
};

class CondWaiter {
public:
    CondWaiter(Sync& sync) : guard_(sync.lock) {
        sync.cond.wait(sync.lock);
    }
private:
    LockGuard guard_;
};

class CondNotifier {
public:
    CondNotifier(Sync& sync) : sync_(sync), guard_(sync.lock) {
    }
    ~CondNotifier() {
        sync_.cond.notify();
    }
private:
    Sync& sync_;
    LockGuard guard_;
};

#endif // CONDITION_H

