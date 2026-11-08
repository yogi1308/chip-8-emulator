#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 4096
#define PC_START_ADDRESS 0x200
#define FONT_SET_START_ADDRESS 0x50
#define FONTSET_SIZE 80
#define NUM_REGISTERS 16

typedef struct chip8 {
    uint16_t pc;
    uint16_t opcode;
    uint8_t memory[MEMORY_SIZE];
    uint8_t registers[NUM_REGISTERS];
    uint16_t I;

} chip8;

chip8 init_chip8(void) {
    chip8 c8 = {0};
    c8.pc = PC_START_ADDRESS;
    return c8;
}

typedef struct decoded_opcode {
    uint16_t op;
    int first_nibble;
    int x;
    int y;
    int n;
    uint8_t nn;
    uint16_t nnn; // lower 4 bits are zeros
} decoded_opcode;

void load_fontset(uint8_t *memory) {
    uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F

    };
    for (int i = 0; i < FONTSET_SIZE; i++) { // load font into memory
        memory[FONT_SET_START_ADDRESS + i] = fontset[i];
    }
}

int load_rom(uint8_t *memory, int argc, char *argv[]) {
    // load program into memory
    if (argc == 1) {
        fprintf(stderr, "No arguments passed. Needs the name of the file to be passed as an argument\n");
        return 1;
    }

    FILE *fp;
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "File pointer is null. File may not exist\n");
        return 1;
    }

    int c;
    int index = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (index >= (MEMORY_SIZE - PC_START_ADDRESS)) { // prevents memory overflow
            fclose(fp);
            return 0;
        }
        memory[PC_START_ADDRESS + index] = c;
        index++;
    }
    fclose(fp);
    return 0;
}

uint16_t fetch(uint8_t *memory, uint16_t pc) { return (memory[pc] << 8) | memory[pc + 1]; }

void print_decoded_opcode(decoded_opcode d_op, uint16_t opcode) {
    printf("\n\nopcode = 0x%04X\n", opcode);
    printf("first_nibble = 0x%01X\n", d_op.first_nibble);
    printf("x = 0x%01X\n", d_op.x);
    printf("y = 0x%01X\n", d_op.y);
    printf("n = 0x%01X\n", d_op.n);
    printf("nn = 0x%02X\n", d_op.nn);
    printf("nnn = 0x%04X\n\n", d_op.nnn);
}

decoded_opcode decode(uint16_t opcode) {
    int first_nibble = (opcode >> 12);
    int x = (opcode >> 8) & 0x0F; // 2nd nibble used to look up one of the 16 8-bit registers
    int y = (opcode >> 4) & 0x0F; // 3rd nibble also used to look up one of the 16 registers
    int n = opcode & 0x0F;        // The fourth nibble. A 4-bit number.
    uint8_t nn = opcode & 0x00FF; // The second byte (third and fourth nibbles). An 8-bit immediate number.
    // upper(little endian) 4bits are zeroes. The second, third and fourth nibbles. A 12-bit immediate memory address.
    uint16_t nnn = opcode & 0x0FFF;

    decoded_opcode d_op;
    d_op.op = opcode;
    d_op.first_nibble = first_nibble;
    d_op.x = x;
    d_op.y = y;
    d_op.n = n;
    d_op.nn = nn;
    d_op.nnn = nnn;
    return d_op;
}

int execute(decoded_opcode d_op, chip8 *c8) {
    switch (d_op.first_nibble) {
        case 0x1:
            c8->pc = d_op.nnn;
            return 0;
        case 0x6:
            c8->registers[d_op.x] = d_op.nn;
            return 0;
        case 0x7:
            c8->registers[d_op.x] = c8->registers[d_op.x] + d_op.nn;
            return 0;
        case 0xA:
            c8->I = d_op.nnn;
            return 0;
        default:
            printf("Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
            return 1;
    }
}

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
