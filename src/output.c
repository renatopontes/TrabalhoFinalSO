#include "output.h"
#include "memory_manager.h"

void print_help() {
    printf(
        "Comando    Descrição\n"
        "q, quit    Termina o programa\n\n"
        "h, help    Exibe esta mensagem\n\n"
        "i, info    Exibe informações sobre o sistema\n\n"
        "mem        Exibe os frames da memória\n\n"
        "paget      Exibe tabela de páginas do processo <id>\n"
        "           id: PID do processo\n\n"
        "s, step    Continua a simulação por <t> unidades de tempo\n"
        "           t: quantidade de tempo para rodar. (default: 1)\n\n"
        "l, load    Carrega na memória um processo de tamanho <b>\n"
        "           b: tamanho em bytes do processo (default: pseudo-aleatório)\n\n"
        "r, ref     O processo <id> faz referência a um endereço\n"
        "             lógico <p> <o> OU relativo <r>\n"
        "           id: PID do processo\n"
        "           p: índice da página\n"
        "           o: offset dentro da página\n"
        "           r: endereço relativo\n"
        );
}

void print_system_info() {
    printf(
        "Informações do sistema\n"
        "Tamanho da memória principal: %d bytes\n"
        "Tamanho do frame: %d bytes\n"
        "Número de frames: %d\n"
        , MEM_SIZE, FRAME_SIZE, N_FRAMES
    );
}

void print_memory() {
    printf("Memória principal\n=================\n");
    for (size_t i = 0; i < N_FRAMES; i += FRAMES_PER_LINE) {
        printf("Frame    ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < N_FRAMES; ++j) {
            printf("%4d ", j);
        }
        puts("");
        printf("PID      ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < N_FRAMES; ++j) {
            if (mem->used_frames[j] != -1)
                printf("%4d ", mem->used_frames[j]);
            else
                printf("  -- ");
        }
        puts("\n");
    }
}

void print_page_table(pid_t pid) {
    printf("Tabela de páginas de P%03d\n=========================\n", pid);
    if (pid >= proc_table->size || !proc_table->table[pid]) {
        printf("   ✘   P%03d não está na memória\n", pid);
        return;
    }
    Process *proc = proc_table->table[pid];
    size_t page_table_size = proc->page_table->size;
    for (size_t i = 0; i < page_table_size; i += FRAMES_PER_LINE) {
        printf("Page     ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < page_table_size; ++j) {
            printf("%4d ", j);
        }
        puts("");
        printf("Frame    ");
        for (frame_t j = i; j < i+FRAMES_PER_LINE && j < page_table_size; ++j) {
            if (proc->page_table->table[j] != -1)
                printf("%4d ", proc->page_table->table[j]);
            else
                printf("  -- ");
        }
        puts("\n");
    }
}

void print_time(char end[]) {
    printf("[%02d:%02d]%s", step / 60, step % 60, end);
}

void prompt_loop() {
    char buff[BUFFER_SIZE];
    char cmd[BUFFER_SIZE] = {'\0'};

    // system("clear");
    print_system_info(mem);
    puts("");
    while (1) {
        print_time("");
        printf("> ");
        CHECK_PTR(fgets(buff, BUFFER_SIZE, stdin));
        buff[strcspn(buff, "\n")] = '\0';

        sscanf(buff, "%s", cmd);

        if (!strcmp(cmd, "q") || !strcmp(cmd, "quit"))
            break;
        else if (!strcmp(cmd, "h") || !strcmp(cmd, "help"))
            print_help();
        else if (!strcmp(cmd, "i") || !strcmp(cmd, "info"))
            print_system_info();
        else if (!strcmp(cmd, "l") || !strcmp(cmd, "load")) {
            size_t proc_size = 0;
            sscanf(&buff[strlen(cmd)], "%lu", &proc_size);
            proc_load(proc_size);
        }
        else if (!strcmp(cmd, "r") || !strcmp(cmd, "ref")) {
            // addr_t logical;
            pid_t id;
            addr_t a, b;
            if (sscanf(&buff[strlen(cmd)], "%d %lu %lu", &id, &a, &b) == 3);
            else translate_relative_address(id, a);
        }
        else if (!strcmp(cmd, "s") || !strcmp(cmd, "step")) {
            uint32_t step_bak = step;
            uint32_t jmp = 1;
            sscanf(&buff[strlen(cmd)], "%u", &jmp);
            update();
            while (step < step_bak + jmp) {
                step++;
                print_time("\n");
                update();
                usleep(PAUSE_TIME);
            }
        }
        else if (!strcmp(cmd, "mem")) {
            print_memory();
        }
        else if (!strcmp(cmd, "paget")) {
            pid_t pid;
            if (sscanf(&buff[strlen(cmd)], "%d", &pid) == 1)
                print_page_table(pid);
            else
                printf("⮡ Bad option\n");
        }
        else if (!strcmp(cmd, "clear")) {
            system("clear");
        }
        else if (!strcmp(cmd, "")) {}
        else {
            printf("   ✘   Comando inválido. Use h para ver uma lista de comandos válidos.\n");
            cmd[0] = '\0';
        }
    }
}