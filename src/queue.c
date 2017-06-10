#include "queue.h"

void init_queue(Queue **q) {
    *q = (Queue *) malloc(sizeof(Queue));
    CHECK_PTR(*q);

    init_list(&((*q)->list));
}

Process *queue_top(Queue *q) {
    return q->list->first->proc;
}

void queue_pop(Queue *q) {
    list_remove_fst(q->list);
}

void queue_push(Queue *q, Process *proc) {
    Node *n;
    init_node(&n, proc);
    list_append(q->list, n);
}

size_t queue_size(Queue *q) {
    return q->list->size;
}