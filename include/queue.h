#ifndef QUEUE_H
#define QUEUE_H

#include "global.h"
#include "list.h"

typedef struct Queue {
	List *list;
} Queue;

void init_queue(Queue **q);
Process *queue_top(Queue *q);
void queue_pop(Queue *q);
void queue_push(Queue *q, Process *proc);
size_t queue_size(Queue *q);

#endif