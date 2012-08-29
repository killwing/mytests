#include <stdexcept>
#include <stdio.h>
#include "CmdServer.h"

int main() {
    try {
        CmdServer server;
        server.start();
    } catch (const std::exception& e) {
        printf("expcetion caught: %s, exit.\n", e.what());
    }
    return 0;
}

