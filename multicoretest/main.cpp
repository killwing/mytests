#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <sched.h>

void showCurCpu(int cpuNum) {
    cpu_set_t get;
    CPU_ZERO(&get);
    //if (pthread_getaffinity_np(pthread_self(), sizeof(get), &get) == -1) {
    if (sched_getaffinity(0, sizeof(get), &get) == -1) {
        printf("warning: cound not get cpu affinity\n");
        exit(1);
    }

    for (int i = 0; i < cpuNum; i++) {
        if (CPU_ISSET(i, &get)) {
            printf("this process %d is running processor : %d\n", getpid(), i);
        }
    }
}

void takeHalfCpu() {
    int interval = 10000; // ms

    while (1) {
        timeval start;
        gettimeofday(&start, NULL);
        
        while (1) {
            timeval end;
            gettimeofday(&end, NULL);
            
            if (1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) > interval) {
                break;
            }

        }

        usleep(interval);
    }
}


int main(int argc, char* argv[]) {

    int myid = -1;
    if (argc == 2) {
        myid = atoi(argv[1]);
    }

    int cpuNum = sysconf(_SC_NPROCESSORS_CONF);
    printf("system has %i processor(s).\n", cpuNum);

    if (myid > cpuNum) {
        printf("CPU core num is out of range.\n");
        exit(1);
    }
    
    if (myid != -1) {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(myid, &set);

        //if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) == -1) { // for pthread
        if (sched_setaffinity(0, sizeof(set), &set) == -1) {
            printf("warning: could not set CPU affinity\n");
            exit(1);
        }
    } else {
        printf("CPU core is not specified.\n");
    }

    takeHalfCpu();
    //while (1) {
    //    showCurCpu(cpuNum); 
    //}
    return 0;
}

