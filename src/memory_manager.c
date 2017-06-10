#include "memory_manager.h"

void allocate_frames(Process *proc) {
    size_t k = 0;
    for (frame_t i = 0; i < N_FRAMES && k < proc->page_table->size; ++i) {
        if (mem->used_frames[i] == -1) {
            mem->used_frames[i] = proc->pid;
            proc->page_table->table[k] = i;
            k++;
            mem->free_frames--;
        }
    }
}

void deallocate_frames(Process *proc) {
    for (frame_t i = 0; i < proc->page_table->size; ++i) {
        mem->used_frames[proc->page_table->table[i]] = -1;
        mem->free_frames++;
    }
}

void init_memory() {
    mem = (Memory *) malloc(sizeof(Memory));
    CHECK_PTR(mem);
    mem->used_frames = (pid_t *) malloc(N_FRAMES * sizeof(pid_t));
    CHECK_PTR(mem->used_frames);
    memset(mem->used_frames, -1, N_FRAMES * sizeof(pid_t));
    mem->free_frames = N_FRAMES;
}

void init_page_table(Process *proc) {
    proc->page_table = (Page_table *) malloc(sizeof(Page_table));
    CHECK_PTR(proc->page_table);
    proc->page_table->size = 1 + (proc->proc_size - 1)/FRAME_SIZE;
    proc->page_table->table = (frame_t *) malloc(proc->page_table->size * sizeof(frame_t));
    CHECK_PTR(proc->page_table->table);
    memset(proc->page_table->table, -1, proc->page_table->size * sizeof(frame_t));
}

void init_proc_table() {
    proc_table = (Process_table *) malloc(sizeof(Process_table));
    CHECK_PTR(proc_table);
    proc_table->size = 0;
    proc_table->table = (Process **) calloc(4, sizeof(Process *));
    CHECK_PTR(proc_table->table);
    proc_table->allc = 4;
    proc_table->first_available_pid = 0;
}

void init_process(Process **proc, size_t size) {
    *proc = (Process *) malloc(sizeof(Process));
    CHECK_PTR(*proc);

    (*proc)->proc_size = size;
    (*proc)->exec_time = rand() % MAX_EXEC_TIME + 1;
    (*proc)->start_time = step;
    init_page_table(*proc);
}

void proc_load(size_t size) {
    Process *proc;

    if (!size && mem->free_frames)
        size = rand() % ((mem->free_frames * FRAME_SIZE)/2) + 1;
    else if (!mem->free_frames) {
        printf("   ✘   Sem memória disponível\n");
        return;
    }

    init_process(&proc, size);
    proc_table_add(proc);
    printf("   ✔   Novo processo criado: PID %d [%d->%d]\n", proc->pid, proc->start_time, proc->start_time + proc->exec_time);

    if (proc->page_table->size <= mem->free_frames) {
        allocate_frames(proc);
        printf("   🛈   %lu frames livres\n", mem->free_frames);
    } else {
        printf("   ✘   Memória cheia\n");
    }
}

void proc_table_add(Process *proc) {
    pid_t newpid = proc_table->first_available_pid;
    if (newpid < proc_table->size) {
        proc_table->table[newpid] = proc;
        proc->pid = newpid;
    }
    else if (proc_table->size < proc_table->allc) {
        proc_table->table[newpid] = proc;
        proc->pid = newpid;
    } else {
        size_t new_allc = (int)(proc_table->allc * GROWTH_RATE);
        Process **pt = (Process **) realloc(proc_table->table,  new_allc * sizeof(Process *));
        CHECK_PTR(pt);
        memset(&pt[proc_table->allc], 0, (new_allc - proc_table->allc) * sizeof(Process *));
        proc_table->table = pt;
        proc_table->allc = new_allc;
        proc_table->table[newpid] = proc;
        proc->pid = newpid;
    }
    proc_table->size++;

    while(newpid < proc_table->size && proc_table->table[newpid])
        newpid++;
    proc_table->first_available_pid = newpid;
}

void update() {
    size_t newsize = proc_table->size;
    for (size_t i = 0, k = 0; k < proc_table->size; i++) {
        Process *proc = proc_table->table[i];
        if (proc) k++;
        if (proc && proc->exec_time + proc->start_time <= step) {
            Page_table *pt = proc->page_table;
            printf("   🛈   Processo %d terminou. Desalocando %lu frame%s.\n", proc->pid, pt->size, pt->size > 1 ? "s" : "");
            deallocate_frames(proc_table->table[i]);
            free(proc_table->table[i]);
            proc_table->table[i] = NULL;
            newsize--;
            if (i < proc_table->first_available_pid)
                proc_table->first_available_pid = i;
            printf("   🛈   %lu frames livres\n", mem->free_frames);
        }
    }
    proc_table->size = newsize;
}



void translate_relative_address(pid_t pid, addr_t rel_addr) {
    addr_t page = rel_addr >> FRAME_SIZE_PWR;
    addr_t offset = rel_addr & (FRAME_SIZE-1);

    if (page*FRAME_SIZE + offset >= proc_table->table[pid]->proc_size) {
        printf("   ✘   P%03d: segmentation fault\n", pid);
        return;
    }
    frame_t frame = proc_table->table[pid]->page_table->table[page];
    printf("   ✔   Physical address: (%lu, %lu)\n", (uint64_t)frame, (uint64_t)offset);
    // return (rel_addr & ~(FRAME_SIZE-1));
}
