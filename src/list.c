#include "list.h"

void init_node(Node **n, Process *proc) {
    *n = (Node *) malloc(sizeof(Node));
    CHECK_PTR(*n);
    (*n)->proc = proc;
    (*n)->next = (*n)->prev = NULL;
}

void init_list(List **l) {
    *l = (List *) malloc(sizeof(List));
    CHECK_PTR(*l);
    (*l)->first = (*l)->last = NULL;
    (*l)->size = 0;
}

void list_append(List *l, Node *n) {
    if (!l->size) {
        l->first = l->last = n;
    } else {
        l->last->next = n;
        l->last->next->prev = l->last;
        l->last = n;
    }
    l->size++;
}

void list_remove_fst(List *l) {
    if (!l->size) return;
    if (l->first->next) {
        Node *old = l->first;
        l->first = l->first->next;
        l->first->prev = NULL;
        free(old);
    } else {
        free(l->first);
        l->first = l->last = NULL;
    }
    l->size--;
}

void list_destroy(List *l) {
    while (l->size) list_remove_fst(l);
    free(l);
}