#ifndef CMDWORKER_H
#define CMDWORKER_H

#include <pthread.h>

class CmdBuffer;

class CmdWorker {
public:
    CmdWorker(char type, CmdBuffer* buffer);
    ~CmdWorker();

    void run();
private:
    pthread_t workThrd_;
    CmdBuffer* buffer_;
    char type_;
};

#endif // CMDWORKER_H

