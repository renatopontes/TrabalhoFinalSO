#ifndef LIST_H
#define LIST_H

#include "global.h"

typedef struct Node {
	Process *proc;
	struct Node *next;
	struct Node *prev;
} Node;

typedef struct List {
	Node *first;
	Node *last;
	size_t size;
} List;

void init_node(Node **n, Process *proc);
void init_list(List **l);
void list_append(List *l, Node *n);
void list_remove_fst(List *l);
void list_destroy(List *l);

#endif