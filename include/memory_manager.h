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
void proc_load(size_t size);
void proc_table_add(Process *proc);
void translate_relative_address(pid_t pid, addr_t rel_addr);
void update();

#endif