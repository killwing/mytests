#include "l.h"

vector<string> Lib::libs_;

void foo()
{
    cout<<"l's foo"<<endl;
}

void
Lib::addtolib(const string& name)
{
    libs_.push_back(name);
}
