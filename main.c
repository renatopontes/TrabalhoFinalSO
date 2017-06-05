#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PAGE_SIZE 65536
#define MAX_PAGES 256 
#define MAX_MEMMORY MAX_PAGES * PAGE_SIZE

typedef struct _tableEntry {
	uint8_t data[PAGE_SIZE]; 
} tableEntry;

tableEntry table[MAX_PAGES];
uint8_t usedPages[MAX_PAGES];
void initTable();
void printTable();

int getNextFreePage();
uint8_t getTableData(char *address);
void insertTableData(char *address, uint8_t byte);

int calculateProcessSize(char *p_name); //
void allocateProcess(char *p_name);
void deallocateProcess(char *p_name);

void decToBin(size_t num, char *output);

int main(int argc, char const *argv[]) {
	initTable(table, MAX_PAGES);

	return 0;
}

void initTable() {
	for (int i = 0; i < MAX_PAGES; i++) {
		table[i].data[0]
		usedPages[0] = 0;
	}
}

int getNextFreePage() {
	for (uint8_t i = 0; i < MAX_PAGES; i++) {
		if (usedPages[i] == 0)
			return i;
	}
	return -1;
}

void allocateProcess(char *p_name) {
	int processSize = calculateProcessSize(p_name), page;
	char bin[9];
	if (processSize != 0) {
		while (processSize > 0) {
			page = getNextFreePage();
			decToBin(page, bin);
			for (int i = 0; i < min(processSize, PAGE_SIZE); i++)
				insertTableData();
		}
	}
}

int calculateProcessSize(char *p_name) {
	if (strcmp(p_name, "process1") == 0) {
		return 1125;
	}
	if (strcmp(p_name, "process2") == 0) {
		return 235872;
	}
	return 0;
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