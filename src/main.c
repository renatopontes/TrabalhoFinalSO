#include "global.h"
#include "memory_manager.h"
#include "prompt.h"
#include "queue.h"

uint32_t step = 0;
uint8_t threads = 0;
uint8_t policy = DENY;
Memory *mem;
Process_table *proc_table;
Queue *queue;

int main(int argc, char const *argv[]) {

    if ((argc >= 2 && !strcmp(argv[1], "wait")) || (argc >= 3 && !strcmp(argv[2], "wait")) ) {
        policy = WAIT;
    }
    if ((argc >= 2 && !strcmp(argv[1], "thread")) || (argc >= 3 && !strcmp(argv[2], "thread")) ) {
        threads = 1;
    }

    srand(time(NULL));
    init_memory(&mem);
    init_proc_table();
    if (threads)
        init_proc_concurrency();
    init_queue(&queue);
    prompt_loop();

    return 0;
}