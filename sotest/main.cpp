#include <string>
#include <iostream>
#include <dlfcn.h>
#include "l.h"
using namespace std;


void foo()
{
    cout<<"main's foo"<<endl;
}

int main()
{
    cout<<"enter main()"<<endl;

    void* ptr = dlopen("libl.so", RTLD_NOW | RTLD_GLOBAL);
    if (!ptr)
    {
        cout<<"load libl err: "<<dlerror()<<endl;
        return 1;
    }

    ptr = dlopen("liba.so", RTLD_NOW | RTLD_GLOBAL);
    if (!ptr)
    {
        cout<<"load liba err: "<<dlerror()<<endl;
        return 1;
    }

    ptr = dlopen("libb.so", RTLD_NOW | RTLD_GLOBAL);
    if (!ptr)
    {
        cout<<"load libb err: "<<dlerror()<<endl;
        return 1;
    }

    cout<<"load all done."<<endl;

    foo();

    //Lib l;
    //l.show();
    
}
