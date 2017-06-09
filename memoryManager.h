#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#ifndef __GUARD__MEMORYMANAGER_
#define __GUARD__MEMORYMANAGER_ 1

#define PAGE_SIZE 65536
#define MAX_PAGES 256 
#define MAX_PROCESS_PAGES 8
#define MAX_PROCESS 64
#define MAX_MEMORY MAX_PAGES * PAGE_SIZE
#define SWAP_AREA "./swap_area"

typedef struct _tableEntry {
	uint8_t data[PAGE_SIZE]; 
} tableEntry;

typedef struct _processTable {
    uint8_t pagesUsed, nextFree;
    int pages[MAX_PAGES]; // swapedPages, swapPageId[MAX_PAGES];
    // clock_t timeOfPages[MAX_PAGES];
    // FILE *swap;
} processTable;

tableEntry table[MAX_PAGES];
processTable processTables[MAX_PROCESS];
uint8_t usedPages[MAX_PAGES];
uint8_t nextFreePage;

void initTable();
void printTable();

uint8_t getTableData(char *address);
void insertTableData(uint8_t page);

int getNextFreePage();
void allocateProcess(int pid);
void deallocateProcess(int pid);
int allocatePage(int pid, uint8_t page);
void deallocatePage(int pid, uint8_t page);

void decToBin(size_t num, char *output);

void swapOutProcess(int pid);
void swapInProcess(int pid, int page);

int getLRUPage(int pid);
void updateLRUPage(int pid);

#endif