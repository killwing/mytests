#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <signal.h>
using namespace std;

static void sigpipe_handler(int sig) {
    cout << "sig PIPE caught" << endl;
}

int main() {
	signal(SIGPIPE, &sigpipe_handler);

    const char* path = "test.pipe";

    mode_t mode = 0666;
    int ret = mkfifo(path, mode);
    if (ret != 0) {
        cout << "pipe exists" << endl;
    }

    std::ofstream out(path, std::ofstream::binary);
	if (!out) {
        cout << "failed to open pipe to write" << endl;
        return 1;
    }
    cout << "opened pipe" << endl;

    sleep(10);

    cout << "writing data" << endl;

    out << "test data";
    out.close();
}
