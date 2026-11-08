#include "../headers/dex.h"
#include "../chip8.h"
#include <stdint.h>
#include <stdio.h>
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

