#include "memoryManager.h"
#include "process.h"

#define N_PROCESS 30

int main(int argc, char const *argv[]) {
	srand(time(NULL));
	initTable();
	initProcessLog();
	for (int i = 0; i < N_PROCESS; i++) {
		initProcess();
	}
	joinProcesses();
	printf("---END---\n");
	// allocateProcess(1);
	// allocateProcess(2);
	// allocateProcess(3);
	// allocateProcess(4);
	// allocateProcess(5);
	// allocateProcess(6);
	// allocateProcess(7);
	// allocateProcess(8);
	// allocateProcess(9);
	// allocateProcess(10);
	// allocateProcess(11);
	// allocateProcess(12);
	// allocateProcess(13);
	// allocateProcess(14);
	// allocateProcess(15);
	// allocateProcess(16);
	// allocateProcess(17);
	// allocateProcess(18);
	// allocateProcess(19);
	// allocateProcess(20);
	// allocateProcess(21);
	// allocateProcess(22);
	// allocateProcess(23);
	// allocateProcess(24);
	// allocateProcess(25);
	// allocateProcess(26);
	// allocateProcess(27);
	// allocateProcess(28);
	// allocateProcess(29);
	// allocateProcess(30);
	// allocateProcess(31);
	// allocateProcess(32);
	// allocateProcess(33);
	// allocateProcess(34);
	// deallocateProcess(31);
	// allocateProcess(35);
	return 0;
}
