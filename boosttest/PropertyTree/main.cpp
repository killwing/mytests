// boost 1.49

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
using namespace std;
using namespace boost;

int main() {
    using property_tree::ptree;

    ptree pt;
    pt.add("name", "jack");
    pt.add("name", "billy");
    pt.put("age", 20);

    pt.add("debug.modules.module", "m1");
    pt.add("debug.modules.module", "m2");
    pt.add("debug.modules.module", 123);

    string name = pt.get<string>("name");
    int age = pt.get<int>("age");
    cout<<name<<age<<endl;

    ptree t = pt.get_child("");
    for (ptree::iterator it = t.begin(); it != t.end(); ++it) {
        cout<<it->first.data()<<endl;
        cout<<it->second.data()<<endl;
    }

    return 0;
}
