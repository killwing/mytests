#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

int main() {
    const char* path = "test.pipe";
    /*
    mode_t mode = 0666;
    int ret = mkfifo(path, mode);
    if (ret != 0) {
        cout << "failed to create pipe" << endl;
        return ret;
    }
    */

    ifstream in(path);
    if (!in) {
        cout << "failed to open pipe to read" << endl;
        return 1;
    }
    cout << "opened pipe" << endl;

    //string str((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    //cout << "read: " << str << endl;
    in.close();
}
