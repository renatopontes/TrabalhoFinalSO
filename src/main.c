#include "global.h"
#include "memory_manager.h"
#include "prompt.h"
#include "queue.h"

uint32_t step = 0;
uint8_t policy = DENY;
Memory *mem;
Process_table *proc_table;
Queue *queue;

int main(int argc, char const *argv[]) {

    if (argc == 2 && !strcmp(argv[1], "wait")) {
        policy = WAIT;
    }

    srand(time(NULL));
    init_memory(&mem);
    init_proc_table();
    init_proc_concurrency();
    init_queue(&queue);
    prompt_loop();

    return 0;
}