#include "global.h"
#include "memory_manager.h"
#include "output.h"
#include "queue.h"

uint32_t step = 0;
Memory *mem;
Process_table *proc_table;

int main(int argc, char const *argv[]) {

	srand(time(NULL));
	init_memory(&mem);
    init_proc_table();
    prompt_loop();

	return 0;
}