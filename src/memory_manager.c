#include "memory_manager.h"
#include "prompt.h"

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

    mem->processes++;
    proc->start_time = step;
}

void deallocate_frames(Process *proc) {
    for (frame_t i = 0; i < proc->page_table->size; ++i) {
        mem->used_frames[proc->page_table->table[i]] = -1;
        mem->free_frames++;
    }
    mem->processes--;
}

void init_memory() {
    mem = (Memory *) malloc(sizeof(Memory));
    CHECK_PTR(mem);

    mem->used_frames = (pid_t *) malloc(N_FRAMES * sizeof(pid_t));
    CHECK_PTR(mem->used_frames);
    memset(mem->used_frames, -1, N_FRAMES * sizeof(pid_t));
    
    mem->processes = 0;
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

    proc_table->table = (Process **) malloc(4 * sizeof(Process *));
    CHECK_PTR(proc_table->table);
    proc_table->allc = 4;
    for (int i = 0; i < proc_table->allc; proc_table->table[i++] = NULL);

    proc_table->first_available_pid = 0;
}

void init_process(Process **proc, size_t size) {
    *proc = (Process *) malloc(sizeof(Process));
    CHECK_PTR(*proc);

    (*proc)->proc_size = size;
    (*proc)->exec_time = rand() % MAX_EXEC_TIME + 1;
    (*proc)->start_time = -1;

    init_page_table(*proc);
}

void proc_load(size_t size) {
    Process *proc;
    size_t free_memory = mem->free_frames * FRAME_SIZE;

    if (!size)
        size = rand() % (free_memory > 1 ? free_memory/2 : (MEM_SIZE/2)) + 1;

    if (size > free_memory && (policy == DENY || size > MEM_SIZE) ) {
        printf(INFO_ERR "Sem memória disponível. O processo não será iniciado.\n");
        return;
    }

    init_process(&proc, size);
    proc_table_add(proc);
    printf(INFO_OK "Novo processo P%03d criado (%lu B, %lu páginas, %d seg)\n",
        proc->pid, proc->proc_size, proc->page_table->size, proc->exec_time);

    if (proc->page_table->size <= mem->free_frames) {
        allocate_frames(proc);
        printf(INFO_OK "P%03d está carregado na memória.\n", proc->pid);
        printf(INFO_WARN "%lu frames livres\n", mem->free_frames);
    } else {
        printf(INFO_WARN "P%03d foi colocado na fila.\n", proc->pid);
        queue_push(queue, proc);
    }
}

void proc_table_add(Process *proc) {
    pid_t newpid = proc_table->first_available_pid;

    if (newpid < proc_table->allc) {
        proc_table->table[newpid] = proc;
        proc->pid = newpid;
    } else {
        size_t new_allc = (int)(proc_table->allc * GROWTH_RATE);
        Process **pt = (Process **) realloc(proc_table->table,  new_allc * sizeof(Process *));
        CHECK_PTR(pt);
        proc_table->table = pt;
        for (int i = proc_table->allc; i < new_allc; proc_table->table[i++] = NULL);
        proc_table->allc = new_allc;
        proc_table->table[newpid] = proc;
        proc->pid = newpid;
    }
    proc_table->size++;

    while(newpid < proc_table->allc && proc_table->table[newpid])
        newpid++;
    proc_table->first_available_pid = newpid;
}

void proc_table_remove(Process *proc) {
    if (!proc) return;

    pid_t pid = proc->pid;
    free(proc_table->table[pid]);
    proc_table->table[pid] = NULL;

    proc_table->size--;
}

void translate_relative_address(pid_t pid, addr_t rel_addr) {
    addr_t page = rel_addr >> FRAME_SIZE_PWR;
    addr_t offset = rel_addr & (FRAME_SIZE-1);

    printf(INFO_WARN "P%03d referencia endereço %lu\n", pid, (uint64_t)rel_addr);

    if (page*FRAME_SIZE + offset >= proc_table->table[pid]->proc_size) {
        printf(INFO_ERR "P%03d: segmentation fault\n", pid);
        deallocate_frames(proc_table->table[pid]);
        proc_table_remove(proc_table->table[pid]);
        printf(INFO_ERR "P%03d foi terminado\n\n", pid);
        usleep(PAUSE_BETWEEN_INFO);
        return;
    }

    addr_t frame = proc_table->table[pid]->page_table->table[page];
    addr_t abs_addr = (frame << FRAME_SIZE_PWR) + offset;
    printf(INFO_OK "Endereço físico %lu (frame: %lu, offset: %lu)\n\n",
        abs_addr, frame, offset);
    usleep(PAUSE_BETWEEN_INFO);
}

void random_request() {
    uint64_t rel_addr;
    int prob;
    int rand_pid = rand() % proc_table->allc;

    if (!proc_table->size) return;

    while (!proc_table->table[rand_pid] || proc_table->table[rand_pid]->start_time == -1)
        rand_pid = rand() % proc_table->allc;

    Process *rproc = proc_table->table[rand_pid];

    prob = rand() % 100 + 1;
    if (prob <= REF_PROB) {
        prob = rand() % 100 + 1;
        if (prob <= SEGFAULT_PROB)
            rel_addr = rproc->proc_size + rand() % (MEM_SIZE - rproc->proc_size);
        else
            rel_addr = rand() % rproc->proc_size;

        translate_relative_address(rand_pid, rel_addr);
    }
}

void update() {
    Process *next_proc = NULL;
    if (queue_size(queue))
        next_proc = queue_top(queue);

    for (size_t i = 0; i < proc_table->allc && proc_table->size; i++) {
        Process *proc = proc_table->table[i];

        random_request();
        
        if (proc && proc->start_time != -1 && proc->exec_time + proc->start_time <= step) {
            Page_table *pt = proc->page_table;

            printf(INFO_WARN "P%03d terminou. Desalocando %lu frame%s.\n",
                proc->pid, pt->size, pt->size > 1 ? "s" : "");
            
            deallocate_frames(proc);
            proc_table_remove(proc);
            
            if (i < proc_table->first_available_pid)
                proc_table->first_available_pid = i;

            printf(INFO_WARN "%lu frames livres\n\n", mem->free_frames);
            usleep(PAUSE_BETWEEN_INFO);
        }

        while (next_proc && mem->free_frames*FRAME_SIZE >= next_proc->proc_size) {
            Page_table *pt = next_proc->page_table;
            printf(INFO_WARN "Foi liberado espaço para P%03d (%lu B, %lu páginas, %d seg)\n",
                next_proc->pid, next_proc->proc_size, pt->size, next_proc->exec_time);
            queue_pop(queue);
            allocate_frames(next_proc);
            printf(INFO_OK "P%03d está carregado na memória.\n", next_proc->pid);
            printf(INFO_WARN "%lu frames livres\n\n", mem->free_frames);
            if (queue_size(queue))
                next_proc = queue_top(queue);
            else
                next_proc = NULL; 
            usleep(PAUSE_BETWEEN_INFO);
        }
    }
}