#include "l.h"

class B : public Lib 
{
public:
    B();

    string name() { return name_; }
private:
    const char* name_;

};
