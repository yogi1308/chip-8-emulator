#include <stdint.h>
#include <stdio.h>
#include "chip8.h"
#include "headers/loaders.h"
#include "headers/dex.h"

int main(int argc, char *argv[]) {
    chip8 c8 = init_chip8();

    load_fontset(c8.memory);
    if (load_rom(c8.memory, argc, argv) == 1) {
        return 1;
    }

    printf("0x%02X\n", c8.pc);
    int executions = 0;
    while (executions < 20) {
        c8.opcode = fetch(c8.memory, c8.pc);
        c8.pc += 2;
        decoded_opcode d_op = decode(c8.opcode);
        print_decoded_opcode(d_op, c8.opcode);
        if (execute(d_op, &c8) != 0) {
            return 1;
        }
        executions++;
    }

    // print memory
    /* for (int i = 0; i < MEMORY_SIZE; i++) { */
    /*     printf("0x%02X %d\n", memory[i], i); */
    /* } */
    return 0;
}
