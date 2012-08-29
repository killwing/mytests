#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include <pthread.h>
#include <boost/noncopyable.hpp>


class MutexLock : private boost::noncopyable {
public:
    MutexLock() {
        pthread_mutex_init(&mutex_, 0);
    }
    ~MutexLock() {
        pthread_mutex_destroy(&mutex_);
    }

    pthread_mutex_t* get() {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

class LockGuard : private boost::noncopyable {
public:
    explicit LockGuard(pthread_mutex_t& lock) : lock_(&lock) {
        pthread_mutex_lock(lock_);
    };
    explicit LockGuard(MutexLock& lock) : lock_(lock.get()) {
        pthread_mutex_lock(lock_);
    };

    ~LockGuard() {
        pthread_mutex_unlock(lock_);
    };

private:
    pthread_mutex_t* lock_;
};


#endif // MUTEXLOCK_H

