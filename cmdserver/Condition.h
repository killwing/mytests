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
        pthread_cond_broadcast(&cond_);
    }
private:
    pthread_cond_t cond_;
};

#endif // CONDITION_H

