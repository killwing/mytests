#include <iostream>
#include <cassert>
#include <string.h> // memcpy
#include <google/profiler.h>
using namespace std;

int main()
{
    ProfilerStart("testprof");
    int nn = 10000;
    while (nn--)
    {
        int kk = 1000;
        while (kk--)
        {

        }
    }
    ProfilerStop();
    return 0;
}
