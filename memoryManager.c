#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "memoryManager.h"

void initTable() {
	for (int i = 0; i < MAX_PAGES; i++) {
        for (int j = 0; j < PAGE_SIZE; j++) {
		    table[i].data[j] = 0;
        }
		usedPages[i] = 0;
	}
	nextFreePage = 0;
    for (int i = 0; i < MAX_PROCESS; i++) {
        processTables[i].nextFree = 0;
        processTables[i].pagesUsed = 0;
        for (int j = 0; j < MAX_PROCESS_PAGES; j++) {
            processTables[i].pages[j] = -1;
        }
    }
}

void printTable() {
    for (int i = 0; i < MAX_PROCESS; i++) {
        if (processTables[i].pagesUsed > 0)
            printf("Process: %d\n\tPages Used: ", i);
        for (int j = 0; j < processTables[i].pagesUsed; j++) {
            printf("%d ", processTables[i].pages[j]);
        }
        if (processTables[i].pagesUsed > 0)
            printf("\n");        
    }
}

void insertTableData(uint8_t page) {
    for (int i = 0; i < PAGE_SIZE; i++) {
        table[page].data[i] = rand() % 256;
    }
}

int getNextFreePage() {
    int result = nextFreePage;
	nextFreePage = (nextFreePage + 1) % MAX_PAGES;
    if (usedPages[result] == 0) 
    	return result;
    return -1;
}

void dealocatePage(int pid, uint8_t page) {

}

void allocateProcess(int pid) {
	int processSize = rand() % 500000 + 1, page;
	if (processSize != 0) {
		while (processSize > 0) {
			page = getNextFreePage();
            if (page < 0) {
                // deal with no free page;
                break;
            }
            else {
                if (processTables[pid].pagesUsed < MAX_PROCESS_PAGES) {
                    processTables[pid].pages[processTables[pid].nextFree] = page;
                    processTables[pid].pagesUsed++;
                    for (int i = 0; i < MAX_PROCESS_PAGES; i++ ) {
                        if (processTables[pid].pages[i] == -1) {
                            processTables[pid].nextFree = i;
                            break;
                        }
                    }
                    usedPages[page] = 1;
                    insertTableData(page);
                    processSize -= PAGE_SIZE;
                }
                else {
                    printf("Falhou, sem mais páginas disponíveis para o processo.\n");
                }
            }
		}
	}
}

void dealocateProcess(int pid) {
}

void decToBin(size_t num, char *output) {
	int count = 0, aux = num;
	while (aux > 0) {
		aux /= 2;
		count++;
	}
	while (num > 0) {
		output[count] = num % 2;
		num /= 2;
		count--;
	}
}