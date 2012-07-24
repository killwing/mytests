#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    std::vector<int> v;
    v.push_back(10);
    v.push_back(12);
    v.push_back(11);
    auto it = std::find_if(v.cbegin(), v.cend(), [](int i) { return i == 11; });
    cout << *it << endl;
    return 0;
}
