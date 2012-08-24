#include <iostream>

class A {
public:
    A() {
        std::cout << "A::ctor " << std::endl;
    }

    ~A() {
        std::cout << "A::dtor" << std::endl;
    }

    A(const A&) {
        std::cout << "A::cp ctor" << std::endl;
    }

    A(A&&) {
        std::cout << "A::mv ctor" << std::endl;
    }

    A& operator=(const A&) {
        std::cout << "A::operator=" << std::endl;
        return *this;
    }

    A& operator=(A&&) {
        std::cout << "A::mv operator=" << std::endl;
        return *this;
    }

    void printx();

private:
    int x_ { 100 };
};
