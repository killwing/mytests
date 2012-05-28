#include <iostream>
#include <cassert>
#include <string.h> // memcpy
#include <pthread.h>
#include <google/profiler.h>
using namespace std;

void* func(void*) {
    int nn = 50000;
    while (nn--) {
        int kk = 5000;
        while (kk--) {

        }
    }
    return 0;
}

void* func1(void*) {
    int nn = 10000;
    while (nn--) {
        int kk = 1000;
        while (kk--) {

        }
    }
    return 0;
}

void* func2(void*) {
    int nn = 20000;
    while (nn--) {
        int kk = 2000;
        while (kk--) {

        }
    }
    return 0;
}

void* func3(void*) {
    int nn = 30000;
    while (nn--) {
        int kk = 3000;
        while (kk--) {

        }
    }
    return 0;
}

int main() {
    func(0);

    pthread_t thrd1;
    pthread_t thrd2;
    pthread_t thrd3;
    pthread_create(&thrd1, 0, func1, 0);
    pthread_create(&thrd2, 0, func2, 0);
    pthread_create(&thrd3, 0, func3, 0);

    pthread_join(thrd1, 0);
    pthread_join(thrd2, 0);
    pthread_join(thrd3, 0);
    return 0;
}
