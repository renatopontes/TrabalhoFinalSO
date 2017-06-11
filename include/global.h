#ifndef GLOBAL_H
#define GLOBAL_H

#define _GNU_SOURCE

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
#define FRAMES_PER_LINE 12
#define DENY 0
#define WAIT 1
#define PAUSE_STEP 350000
#define PAUSE_BETWEEN_INFO 200000

#define CHECK_PTR(p) {\
	if (!(p)) {\
		printf("ERROR for " #p "@%s: %s\n", __func__, strerror(errno));\
		exit(errno);\
	}\
}

typedef uint64_t addr_t;

#if N_FRAMES <= (1<<7)
typedef int8_t frame_t;
#elif N_FRAMES <= (1<<15)
typedef int16_t frame_t;
#elif N_FRAMES <= (1ll<<31)
typedef int32_t frame_t;
#else
typedef int64_t frame_t;
#endif

typedef struct Memory {
    pid_t *used_frames;
    size_t free_frames;
    size_t processes;
} Memory;

typedef struct Page_table {
	frame_t *table;
	size_t size;
} Page_table;

typedef struct Process {
	pid_t pid;
	size_t proc_size;
	int32_t exec_time;
    int32_t start_time;
	Page_table *page_table;
} Process;

typedef struct Process_table {
    Process **table;
    size_t size;
    size_t allc;
    pid_t first_available_pid;
} Process_table;

typedef struct Queue Queue;

extern uint32_t step;
extern uint8_t policy;
extern Memory *mem;
extern Process_table *proc_table;
extern Queue *queue;

#endif