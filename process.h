#include <pthread.h>

typedef struct _fprocess {
    pthread_t *t;
    void (*func)(void*);
    int pid, processSize;
} fprocess;

fprocess* initProcess();
void initProcessLog();
void stopProcess(fprocess *p);
void joinProcesses();