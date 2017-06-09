#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
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
        for (int j = 0; j < MAX_PAGES; j++) {
            processTables[i].pages[j] = -1;
        }
        // processTables[i].swap = NULL;
        // processTables[i].swapedPages = 0;
        // for (int j = 0; j < MAX_PAGES; j++) {
        //     processTables[i].swapPageId[j] = -1;
        // }
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
    if (usedPages[result] == 0) {
    	return result;
    }
    else {
        for (int i = 0; i < MAX_PAGES; i++) {
            if (usedPages[i] == 0)
                return i;
        }
    }
    return -1;
}


int allocatePage(int pid, uint8_t page) {
    if (usedPages[page] != 0) {
        printf("Page fault! Process %d on page %u.\n", pid, page);
        return 0;
    }
    else {
        processTables[pid].pages[processTables[pid].nextFree] = page;
        processTables[pid].pagesUsed++;
        // processTables[pid].timeOfPages[processTables[pid].nextFree] = clock();
        for (int i = 0; i < MAX_PAGES; i++ ) {
            if (processTables[pid].pages[i] == -1) {
                processTables[pid].nextFree = i;
                break;
            }
        }
        usedPages[page] = 1;
        insertTableData(page);
        return 1;
    }
}

void deallocatePage(int pid, uint8_t page) {
    for (int i = 0; i < processTables[pid].pagesUsed; i++) {
        if (processTables[pid].pages[i] == page) {
            usedPages[page] = 0;
            int j = i;
            while (j < MAX_PAGES - 1 || (processTables[pid].pages[j] != -1 && i != j)) {
                processTables[pid].pages[j] = processTables[pid].pages[j + 1];
                j++;
            }
            processTables[pid].nextFree--;
        }
    }
    processTables[pid].pagesUsed--;    
}

void allocateProcess(int pid) {
	int processSize = rand() % 1000000 + 1, page;
	if (processSize != 0) {
		while (processSize > 0) {
			page = getNextFreePage();
            while (page < 0) {
                sleep(1);
    			page = getNextFreePage();                
            }
            processTables[pid].pages[processTables[pid].nextFree] = page;
            processTables[pid].pagesUsed++;
            // processTables[pid].timeOfPages[processTables[pid].nextFree] = clock();
            for (int i = 0; i < MAX_PAGES; i++ ) {
                if (processTables[pid].pages[i] == -1) {
                    processTables[pid].nextFree = i;
                    break;
                }
            }
            usedPages[page] = 1;
            insertTableData(page);
            processSize -= PAGE_SIZE;
		}
	}
}

void deallocateProcess(int pid) {
    for (int i = 0; i < processTables[pid].pagesUsed; i++) {
        usedPages[processTables[pid].pages[i]] = 0;
        if (nextFreePage > processTables[pid].pages[i])
            nextFreePage = processTables[pid].pages[i];
        processTables[pid].pages[i] = -1;
    }
    processTables[pid].pagesUsed = 0;
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
// ignore swap and LRU, but well, i'll leave it here.

// int getLRUPage(int pid) {
//     clock_t c = clock();
//     int result = -1;
//     for (int i = 0; i < processTables[pid].pagesUsed; i++) {
//         if (processTables[pid].timeOfPages[i] < c) {
//             c = processTables[pid].timeOfPages[i];
//             result = i;
//         }
//     }
//     return result;
// }

// void swapOutProcess(int pid) {
//     int page = getLRUPage(pid);
//     printf("swapping...");
//     if (processTables[pid].swap == NULL) {
//         char buffer[10];
//         sprintf(buffer, "%d", pid);
//         const char* filename = strcat(SWAP_AREA, buffer);
//         processTables[pid].swap = fopen(filename, "wb");
//     }
//     fseek(processTables[pid].swap, PAGE_SIZE * processTables[pid].swapedPages, SEEK_SET);
//     fwrite((void *) table[page].data, sizeof(uint8_t), PAGE_SIZE, processTables[pid].swap);
//     processTables[pid].swapPageId[page] = processTables[pid].swapedPages;
//     for (int i = 0; i < MAX_PROCESS_PAGES; i++) {
//         if (processTables[pid].pages[i] == page) {
//             processTables[pid].nextFree = i;
//         }
//     }
//     processTables[pid].swapedPages++;
// }

// void swapInProcess(int pid, int page) {
//     if (processTables[pid].swap != NULL) {
//         if (processTables[pid].swapPageId[page] != -1) {
//             fseek(processTables[pid].swap, PAGE_SIZE * (processTables[pid].swapPageId[page]), SEEK_SET);
//             fread((void *) table[page].data, sizeof(uint8_t), PAGE_SIZE, processTables[pid].swap);
//             processTables[pid].swapedPages--;
//             processTables[pid].swapPageId[page] = -1;
//         }
//     }
// }