#include <iostream>
#include <cassert>
#include <string.h> // memcpy
#include <pthread.h>
#include <google/profiler.h>
using namespace std;

void* func(void*) {
    int* p = new int[500];
    delete[] p;
    return 0;
}

void* func1(void*) {
    int* p = new int[100];
    delete[] p;
    return 0;
}

void* func2(void*) {
    int* p = new int[200];
    delete[] p;
    return 0;
}

void* func3(void*) {
    int* p = new int[300];
    //delete[] p;
    p = 0;
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
