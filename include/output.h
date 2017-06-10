#ifndef OUTPUT_H
#define OUTPUT_H

#define _GNU_SOURCE

#include "global.h"

#define PAUSE_TIME 450000

void print_help();
void print_system_info();
void print_memory();
void print_page_table(pid_t pid);
void print_time(char end[]);
void prompt_loop();

#endif