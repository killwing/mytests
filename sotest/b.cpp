#include <iostream>
#include "b.h"
using namespace std;

static B b;
void foo()
{
    cout<<"b's foo"<<endl;
}

B::B():name_("lib B")
{
    cout<<"B creating..."<<endl;
    addtolib(name_);
    show();
    foo();
}
