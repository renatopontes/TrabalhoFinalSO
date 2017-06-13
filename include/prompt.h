#ifndef OUTPUT_H
#define OUTPUT_H

#include "global.h"
#include "memory_manager.h"

extern size_t cols;

size_t colunas();
void print_separator(char tok);
void print_help();
void print_system_info();
void print_memory();
void print_page_table(pid_t pid);
void print_time(char end[]);
void prompt_loop();
void print_queue();

#endif