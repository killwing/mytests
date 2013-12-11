#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include "a.h"
using namespace std;

int main() {
    ([](auto x, auto y) { cout << x + y << endl; })(1, 2);
    return 0;
}
