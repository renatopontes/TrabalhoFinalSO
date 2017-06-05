#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef __GUARD__MEMORYMANAGER_
#define __GUARD__MEMORYMANAGER_ 1

#define PAGE_SIZE 65536
#define MAX_PAGES 256 
#define MAX_PROCESS_PAGES 8
#define MAX_PROCESS 64
#define MAX_MEMORY MAX_PAGES * PAGE_SIZE

typedef struct _tableEntry {
	uint8_t data[PAGE_SIZE]; 
} tableEntry;

typedef struct _processTable {
    uint8_t pagesUsed, nextFree;
    int pages[MAX_PROCESS_PAGES];
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
void dealocatePage(int pid, uint8_t page);

void decToBin(size_t num, char *output);

#endif