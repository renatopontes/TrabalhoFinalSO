#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ARCH 16
#define MEM_SIZE (1<<ARCH)
#define FRAME_SIZE_PWR 10
#define FRAME_SIZE (1<<FRAME_SIZE_PWR)
#define N_FRAMES (1<<(ARCH-FRAME_SIZE_PWR))
#define BUFFER_SIZE 128
#define GROWTH_RATE 1.5
#define MAX_EXEC_TIME 16
#define LAUNCH_INTERVAL 3
#define FRAMES_PER_LINE 12

#define CHECK_PTR(p) {\
	if (!(p)) {\
		printf("ERROR for " #p "@%s: %s\n", __func__, strerror(errno));\
		exit(errno);\
	}\
}

typedef uint8_t byte_t;

#if N_FRAMES <= (1<<7)
typedef int8_t frame_t;
#elif N_FRAMES <= (1<<15)
typedef int16_t frame_t;
#elif N_FRAMES <= (1ll<<31)
typedef int32_t frame_t;
#else
typedef int64_t frame_t;
#endif

typedef uint64_t addr_t;

typedef enum Action {LAUNCH, WRITE, READ, N_ACTIONS} Action;

typedef struct Memory {
    pid_t *used_frames;
    size_t free_frames;
} Memory;

typedef struct Page_table {
	frame_t *table;
	size_t size;
} Page_table;

typedef struct Process {
	pid_t pid;
	size_t proc_size;
	uint32_t exec_time;
    uint32_t start_time;
    bool running;
	Page_table *page_table;
} Process;

typedef struct Process_table {
    Process **table;
    size_t size;
    size_t allc;
    pid_t first_available_pid;
} Process_table;

uint32_t step = 0;
Memory *mem;
Process_table *proc_table;

void print_time(char end[]);

void init_memory() {
	mem = (Memory *) malloc(sizeof(Memory));
	CHECK_PTR(mem);
    mem->used_frames = (pid_t *) malloc(N_FRAMES * sizeof(pid_t));
    CHECK_PTR(mem->used_frames);
    memset(mem->used_frames, -1, N_FRAMES * sizeof(pid_t));
    mem->free_frames = N_FRAMES;
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

void init_page_table(Process *proc) {
	proc->page_table = (Page_table *) malloc(sizeof(Page_table));
	CHECK_PTR(proc->page_table);
    proc->page_table->size = 1 + (proc->proc_size - 1)/FRAME_SIZE;
	proc->page_table->table = (frame_t *) malloc(proc->page_table->size * sizeof(frame_t));
	CHECK_PTR(proc->page_table->table);
    memset(proc->page_table->table, -1, proc->page_table->size * sizeof(frame_t));
}

void init_process(Process **proc, size_t size) {
	*proc = (Process *) malloc(sizeof(Process));
	CHECK_PTR(*proc);

	(*proc)->proc_size = size;
	(*proc)->exec_time = rand() % MAX_EXEC_TIME + 1;
    (*proc)->start_time = step;
    init_page_table(*proc);
}

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

void translate_relative_address(pid_t pid, addr_t rel_addr) {
    addr_t page = rel_addr >> FRAME_SIZE_PWR;
    addr_t offset = rel_addr & (FRAME_SIZE-1);
    printf("%lu\n", page);
    printf("%lu\n", offset);

    frame_t frame = proc_table->table[pid]->page_table->table[page];
    printf("   ✔   Physical address: (%lu, %lu)\n", (uint64_t)frame, (uint64_t)offset);
    // return (rel_addr & ~(FRAME_SIZE-1));
}

void proc_load(size_t size) {
    Process *proc;

    if (!size && mem->free_frames)
        size = rand() % ((mem->free_frames * FRAME_SIZE)/2) + 1;
    else {
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

void print_help() {
    printf(
        "Comando    Descrição\n"
        "h, help    Exibe esta mensagem\n\n"
        "i, info    Exibe informações sobre o sistema\n\n"
        "mem        Exibe os frames da memória\n\n"
        "paget      Exibe tabela de páginas do processo <id>\n"
        "           id: PID do processo\n\n"
        "q, quit    Termina o programa\n\n"
        "s, step    Continua a simulação por <t> unidades de tempo\n"
        "           t: quantidade de tempo para rodar. (default: 1)\n\n"
        "l, load    Carrega na memória um processo de tamanho <b>\n"
        "           b: tamanho em bytes do processo (default: pseudo-aleatório)\n\n"
        "r, ref     O processo <id> faz referência a um endereço\n"
        "             lógico <p> <o> OU relativo <r>\n"
        "           id: PID do processo\n"
        "           p: índice da página\n"
        "           o: offset dentro da página\n"
        "           r: endereço relativo\n"
        );
}

void print_system_info() {
    printf(
        "Informações do sistema\n"
        "Tamanho da memória principal: %d bytes\n"
        "Tamanho do frame: %d bytes\n"
        "Número de frames: %d\n"
        , MEM_SIZE, FRAME_SIZE, N_FRAMES
    );
}

void print_memory() {
    printf("Memória principal\n=================\n");
    for (size_t i = 0; i < N_FRAMES; i += FRAMES_PER_LINE) {
        printf("Frame    ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < N_FRAMES; ++j) {
            printf("%4d ", j);
        }
        puts("");
        printf("PID      ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < N_FRAMES; ++j) {
            if (mem->used_frames[j] != -1)
                printf("%4d ", mem->used_frames[j]);
            else
                printf("  -- ");
        }
        puts("\n");
    }
}

void print_page_table(pid_t pid) {
    printf("Tabela de páginas de P%03d\n=========================\n", pid);
    if (pid >= proc_table->size || !proc_table->table[pid]) {
        printf("   ✘   P%03d não está na memória\n", pid);
        return;
    }
    Process *proc = proc_table->table[pid];
    size_t page_table_size = proc->page_table->size;
    for (size_t i = 0; i < page_table_size; i += FRAMES_PER_LINE) {
        printf("Page     ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < page_table_size; ++j) {
            printf("%4d ", j);
        }
        puts("");
        printf("Frame    ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < page_table_size; ++j) {
            if (proc->page_table->table[j] != -1)
                printf("%4d ", proc->page_table->table[j]);
            else
                printf("  -- ");
        }
        puts("\n");
    }
}

void print_time(char end[]) {
    printf("[%02d:%02d]%s", step / 60, step % 60, end);
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

int main(int argc, char const *argv[]) {
    char buff[BUFFER_SIZE];
    char cmd[BUFFER_SIZE] = {'\0'};

	srand(time(NULL));
	init_memory(&mem);
    init_proc_table();

    system("clear");
    print_system_info(mem);
    puts("");
    while (1) {
        print_time("");
        printf("> ");
        CHECK_PTR(fgets(buff, BUFFER_SIZE, stdin));
        buff[strcspn(buff, "\n")] = '\0';

        sscanf(buff, "%s", cmd);

        if (!strcmp(cmd, "q") || !strcmp(cmd, "quit"))
            break;
        else if (!strcmp(cmd, "h") || !strcmp(cmd, "help"))
            print_help();
        else if (!strcmp(cmd, "i") || !strcmp(cmd, "info"))
            print_system_info();
        else if (!strcmp(cmd, "l") || !strcmp(cmd, "load")) {
            size_t proc_size = 0;
            sscanf(&buff[strlen(cmd)], "%lu", &proc_size);
            proc_load(proc_size);
        }
        else if (!strcmp(cmd, "r") || !strcmp(cmd, "ref")) {
            // addr_t logical;
            pid_t id;
            addr_t a, b;
            if (sscanf(&buff[strlen(cmd)], "%d %lu %lu", &id, &a, &b) == 3);
            else translate_relative_address(id, a);
        }
        else if (!strcmp(cmd, "s") || !strcmp(cmd, "step")) {
            uint32_t step_bak = step;
            uint32_t jmp = 1;
            sscanf(&buff[strlen(cmd)], "%u", &jmp);
            update();
            while (step < step_bak + jmp) {
                step++;
                print_time("\n");
                update();
                sleep(1);
            }
        }
        else if (!strcmp(cmd, "mem")) {
            print_memory();
        }
        else if (!strcmp(cmd, "paget")) {
            pid_t pid;
            if (sscanf(&buff[strlen(cmd)], "%d", &pid) == 1)
                print_page_table(pid);
            else
                printf("⮡ Bad option\n");
        }
        else if (!strcmp(cmd, "clear")) {
            system("clear");
        }
        else if (!strcmp(cmd, "")) {}
        else {
            printf("   ✘   Comando inválido. Use h para ver uma lista de comandos válidos.\n");
            cmd[0] = '\0';
        }
    }

	return 0;
}