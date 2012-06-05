#include <iostream>
#include <cassert>
#include <string.h> // memcpy
#include <pthread.h>
#include <stdio.h>
#include <gperftools/malloc_extension.h>
using namespace std;

const int M = 1000000;

void* func(void*) {
    char* p = new char[50*M];
    //delete[] p;
    return 0;
}

void* func1(void*) {
    char* p = new char[10*M];
    sleep(3);
    //delete[] p;
    return 0;
}

void* func2(void*) {
    char* p = new char[2*M];
    //delete[] p;
    return 0;
}

void* func3(void*) {
    char* p = new char[60*M];
    memset(p, 0, 60*M);
    return 0;
}

int main(int, char**) {
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
    cout << "exiting" << endl;
    return 0;
}
