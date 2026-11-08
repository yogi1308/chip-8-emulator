#include "../headers/dex.h"

#include <stdint.h>
#include <stdio.h>

#include "../chip8.h"
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
        case 0x0:
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    c8->screen[y][x] = 0;
                }
            }
            return 0;
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
        case 0xD: {
            // sprites are 8 pixel wide and n pixels tall
            uint8_t x = c8->registers[d_op.x] % 64;
            uint8_t y = c8->registers[d_op.y] % 32;
            uint8_t h = d_op.n;
            uint16_t sprite_addr = c8->I;

            for (int i = 0; i < h; i++) {
                int shift = 0;
                uint8_t sprite = c8->memory[sprite_addr + i];
                c8->registers[0xF] = 0;
                for (int j = 0; j < 8; j++) {
                    int curr = c8->screen[y + i][x + j];
                    int bit = (sprite >> (7 - j)) & 0x1;
                    if ((y + i) < 32 && (x + j) < 64) {
                        c8->screen[y + i][x + j] = curr ^ bit;
                        if ((curr ^ bit) == 1) {
                            c8->registers[0xF] = 1;
                        }
                    }
                }
            }
            return 0;
        }
        default:
            printf("Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
            return 1;
    }
}
