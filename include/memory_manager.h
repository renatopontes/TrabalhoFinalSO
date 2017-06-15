#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "global.h"
#include "queue.h"

void allocate_frames(Process *proc);
void deallocate_frames(Process *proc);
void init_memory();
void init_page_table(Process *proc);
void init_proc_table();
void init_process(Process **proc, size_t size);
void init_proc_concurrency();
void proc_thread_func(void *args);
void proc_thread_init(Process *proc);
void proc_load(size_t size);
void proc_table_add(Process *proc);
void proc_table_remove(Process *proc);
void translate_relative_address(pid_t pid, addr_t rel_addr);
void memory_ref(Process *proc);
void random_request();
void update();

extern sem_t *memoryAccess;
extern pthread_mutex_t memoryMutex, processMutex;
extern int total;
extern pid_t stop;

#endif