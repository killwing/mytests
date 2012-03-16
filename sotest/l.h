#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

void foo();

class Lib
{
public:
    void addtolib(const string& name);
    void show()
    {
        copy(libs_.begin(), libs_.end(), ostream_iterator< string >(cout, " | "));
        cout<<endl;
    }
private:
    static vector<string> libs_;
};
