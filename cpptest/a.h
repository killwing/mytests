#include <iostream>

class A {
public:
    A() {
        std::cout<<"A::ctor"<<std::endl;
    }
    ~A() {
        std::cout<<"A::dtor"<<std::endl;
    }
    A(const A&) {
        std::cout<<"A::cpctor"<<std::endl;
    }

    A& operator=(const A&) {
        std::cout<<"operator="<<std::endl;
        return *this;
    }

    void print();
    int foo() {
        std::cout<<"foo"<<std::endl;        
        x_ = 100;
        return 1;
    }
private:
    int x_;
};
