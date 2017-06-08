#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARCH 16
#define MEM_SIZE (1<<ARCH)
#define FRAME_SIZE_PWR 10
#define FRAME_SIZE (1<<FRAME_SIZE_PWR)
#define N_FRAMES (1<<(ARCH-FRAME_SIZE_PWR))

#define CHECK_PTR(p) {\
	if (!(p)) {\
		printf("ERROR for " #p "@%s: %s\n", __func__, strerror(errno));\
		exit(errno);\
	}\
}

typedef enum _action_t {BIND, RELEASE} action_t;

typedef uint8_t byte_t;

#if N_FRAMES <= (1<<8)
typedef uint8_t frame_t;
#elif N_FRAMES <= (1<<16)
typedef uint16_t frame_t;
#elif N_FRAMES <= (1ll<<32)
typedef uint32_t frame_t;
#else
typedef uint64_t frame_t;
#endif

typedef frame_t pid_t;

typedef struct _memory_t {
	byte_t *mem_array;
} memory_t;

typedef struct _page_table_t {
	frame_t *table;
	size_t table_size;
} page_table_t;

typedef struct _process_t {
	pid_t pid;
	size_t proc_size;
	uint32_t exec_time;
	page_table_t *page_table;
} process_t;

void manage_pid(process_t *proc, action_t action) {
	static pid_t new_pid = 0;
	static bool used_pid[N_FRAMES] = {0};

	switch(action) {
		case BIND:
			while (used_pid[new_pid])
				new_pid = new_pid + 1 == N_FRAMES ? 0 : new_pid + 1;
			proc->pid = new_pid;
			break;
		case RELEASE:
			used_pid[proc->pid] = false;
			break;
	}
}

void init_memory(memory_t **mem) {
	*mem = (memory_t *) malloc(sizeof(memory_t));
	CHECK_PTR(*mem);
	(*mem)->mem_array = (byte_t *) malloc(MEM_SIZE * sizeof(byte_t));
	CHECK_PTR((*mem)->mem_array);
}

void init_page_table(process_t *proc) {
	proc->page_table = (page_table_t *) malloc(sizeof(page_table_t));
	CHECK_PTR(proc->page_table);
	proc->page_table->table = (frame_t *) malloc((proc->proc_size - 1)/(FRAME_SIZE + 1) * sizeof(frame_t));
	CHECK_PTR(proc->page_table->table);
}

void init_process(process_t **proc, size_t proc_size) {
	*proc = (process_t *) malloc(sizeof(process_t));
	CHECK_PTR(proc);

	manage_pid(*proc, BIND);
	(*proc)->proc_size = proc_size;
	(*proc)->exec_time = rand() % 16 + 1;
}

int main(int argc, char const *argv[]) {
	process_t *p;
	memory_t *mem;

	srand(time(NULL));
	init_memory(&mem);
	init_process(&p, 1 << 6);
	init_page_table(p);

	return 0;
}