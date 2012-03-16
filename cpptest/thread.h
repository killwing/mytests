#include <pthread.h>

void* 
threadproc(void*) {
    static A a;
    return 0;
}

#define FORK_THREAD(X) \
    pthread_t thrd##X; \
    if (pthread_create(&thrd##X, 0, threadproc, 0)) { return 1; }

#define JOIN_THREAD(X) \
    pthread_join(thrd##X, 0);
