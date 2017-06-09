#include "process.h"
#include "memoryManager.h"
#include <time.h>
#include <unistd.h>

fprocess *processes = NULL;
int qntProcesses = 0;
int stop = 1;
int x = 0;
pthread_t *logger;

void defaultProcess(void *args) {
    fprocess* self = (fprocess *) args;
    uint8_t newPage;
    int notIn = 0;
    while (stop) {
        if (self->processSize > 0) {
            newPage = rand() % MAX_PAGES;
            if (allocatePage(self->pid, newPage))
                self->processSize -= PAGE_SIZE;
        }
        sleep(rand() % 2 + 1);
        if (processTables[self->pid].pagesUsed > 0) {
            if (rand() % 3) {
                newPage = processTables[self->pid].pages[rand() % processTables[self->pid].pagesUsed];
                deallocatePage(self->pid, newPage);
                self->processSize += PAGE_SIZE;
            }
        }
    }
    deallocateProcess(self->pid);
    printf("exiting process %d\n", self->pid);
}

void loggerFunction(void *args) {
    while (stop) {
        printTable();
        usleep(500000);
    }
    printf("Exiting logger.\n");
}

fprocess* initProcess() {
    fprocess *new;
    new = (fprocess*) malloc(sizeof(fprocess));
    new->pid = qntProcesses;
    new->processSize = rand() % 1000000 + 1;
    new->func = &defaultProcess;
    new->t = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(new->t, NULL, (void *)(void *) new->func, (void *) new);
    qntProcesses++;
    processes = realloc(processes, qntProcesses * sizeof(fprocess));
    processes[qntProcesses - 1] = *new;
    return new;
}

void initProcessLog() {
    logger = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(logger, NULL, (void *)(void *) loggerFunction, NULL);
}

void joinProcesses() {
    for (int i = 0; i < qntProcesses; i++) {
        pthread_join(*(processes[i].t), NULL);
        printf("Joined %d.\n", i);
    }
    pthread_join(*logger, NULL);
}