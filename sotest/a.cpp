#include <iostream>
#include "a.h"
#include "l.h"
using namespace std;

static A a;
void foo()
{
    cout<<"a's foo"<<endl;
}

A::A():name_("libA")
{
    cout<<"A creating..."<<endl;
    Lib l;
    l.addtolib(name_);
    l.show();
    foo();
}

