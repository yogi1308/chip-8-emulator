#include "../headers/dex.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
            switch (d_op.nnn) {
                case 0x0E0:
                    for (int y = 0; y < 32; y++) {
                        for (int x = 0; x < 64; x++) {
                            c8->screen[y][x] = 0;
                        }
                    }
                    return 0;
                case 0x0EE:
                    if (c8->stack_pointer == 0) {
                        fprintf(stderr, "Stack is empty when executing 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
                        return 1;
                    }
                    c8->stack_pointer--;
                    c8->pc = c8->stack[c8->stack_pointer];
                    return 0;
                default:
                    fprintf(stderr, "Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
                    return 1;
            }
        case 0x1:
            c8->pc = d_op.nnn;
            return 0;
        case 0x2:
            if (c8->stack_pointer >= 16) {
                fprintf(stderr, "Stack overflowed. Limit is 16 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
                return 1;
            }
            c8->stack[c8->stack_pointer] = c8->pc;
            c8->stack_pointer++;
            c8->pc = d_op.nnn;
            return 0;
        case 0x3:
            if (c8->registers[d_op.x] == d_op.nn) {
                c8->pc += 2;
            }
            return 0;
        case 0x4:
            if (c8->registers[d_op.x] != d_op.nn) {
                c8->pc += 2;
            }
            return 0;
        case 0x5:
            if (c8->registers[d_op.x] == c8->registers[d_op.y]) {
                c8->pc += 2;
            }
            return 0;
        case 0x6:
            c8->registers[d_op.x] = d_op.nn;
            return 0;
        case 0x7:
            c8->registers[d_op.x] = c8->registers[d_op.x] + d_op.nn;
            return 0;
        case 0x8:
            switch (d_op.n) {
                case 0:
                    c8->registers[d_op.x] = c8->registers[d_op.y];
                    return 0;
                case 1:
                    c8->registers[d_op.x] = c8->registers[d_op.y] | c8->registers[d_op.x];
                    return 0;
                case 2:
                    c8->registers[d_op.x] = c8->registers[d_op.y] & c8->registers[d_op.x];
                    return 0;
                case 3:
                    c8->registers[d_op.x] = c8->registers[d_op.y] ^ c8->registers[d_op.x];
                    return 0;
                case 4: {
                    int sum = c8->registers[d_op.y] + c8->registers[d_op.x];
                    c8->registers[d_op.x] = sum;
                    if (sum > 255) {
                        c8->registers[0xF] = 1;
                    } else {
                        c8->registers[0xF] = 0;
                    }
                    return 0;
                }
                case 5: {
                    int flag;
                    if (c8->registers[d_op.x] >= c8->registers[d_op.y]) {
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    int sub = c8->registers[d_op.x] - c8->registers[d_op.y];
                    c8->registers[d_op.x] = sub;
                    c8->registers[0xF] = flag;
                    return 0;
                }
                case 7: {
                    int flag;
                    if (c8->registers[d_op.y] >= c8->registers[d_op.x]) {
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    int sub = c8->registers[d_op.y] - c8->registers[d_op.x];
                    c8->registers[d_op.x] = sub;
                    c8->registers[0xF] = flag;
                    return 0;
                }
                case 6: {
                    if (c8->shift_uses_vy) {
                        c8->registers[d_op.x] = c8->registers[d_op.y];
                    }
                    uint8_t shifted_out_bit = c8->registers[d_op.x] & 1;
                    c8->registers[d_op.x] = c8->registers[d_op.x] >> 1;
                    c8->registers[0xF] = shifted_out_bit;
                    return 0;
                }
                case 0xE: {
                    if (c8->shift_uses_vy) {
                        c8->registers[d_op.x] = c8->registers[d_op.y];
                    }
                    uint8_t shifted_out_bit = c8->registers[d_op.x];
                    c8->registers[d_op.x] = c8->registers[d_op.x] << 1;
                    c8->registers[0xF] = (shifted_out_bit >> 7) & 1;
                    return 0;
                }
                default:
                    fprintf(stderr, "Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
                    return 1;
            }
        case 0x9:
            if (c8->registers[d_op.x] != c8->registers[d_op.y]) {
                c8->pc += 2;
            }
            return 0;
        case 0xA:
            c8->I = d_op.nnn;
            return 0;
        case 0xB:
            if (c8->bxnn) {
                c8->pc = d_op.nnn + c8->registers[d_op.x];
                return 0;
            }
            c8->pc = d_op.nnn + c8->registers[0x0];
            return 0;
        case 0xC:
            c8->registers[d_op.x] = rand() & d_op.nn;
            return 0;
        case 0xD: {
            // sprites are 8 pixel wide and n pixels tall
            uint8_t x = c8->registers[d_op.x] % 64;
            uint8_t y = c8->registers[d_op.y] % 32;
            uint8_t h = d_op.n;
            uint16_t sprite_addr = c8->I;
            c8->registers[0xF] = 0;

            for (int i = 0; i < h; i++) {
                uint8_t sprite = c8->memory[sprite_addr + i];
                for (int j = 0; j < 8; j++) {
                    int bit = (sprite >> (7 - j)) & 0x1;
                    if ((y + i) < 32 && (x + j) < 64) {
                        int curr = c8->screen[y + i][x + j];
                        c8->screen[y + i][x + j] = curr ^ bit;
                        if (curr == 1 && bit == 1) {
                            c8->registers[0xF] = 1;
                        }
                    }
                }
            }
            return 0;
        }
        case 0xE:
            switch (d_op.nn) {
                case 0x9E:
                    if (c8->key_pressed == c8->registers[d_op.x]) {
                        c8->pc += 2;
                    }
                    break;
                case 0xA1:
                    if (c8->key_pressed != c8->registers[d_op.x]) {
                        c8->pc += 2;
                    }
                    break;
                default:
                    fprintf(stderr, "Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
                    return 1;
            }
            return 0;
        case 0xF:
            switch (d_op.nn) {
                case 0x07:
                    c8->registers[d_op.x] = c8->delay_timer;
                    break;
                case 0x15:
                    c8->delay_timer = c8->registers[d_op.x];
                    break;
                case 0x18:
                    c8->sound_timer = c8->registers[d_op.x];
                    break;
                case 0x1E:
                    c8->I += c8->registers[d_op.x];
                case 0x0A:
                    if (c8->key_pressed != -1) {
                        c8->registers[d_op.x] = c8->key_pressed;
                        return 0;
                    }
                    c8->pc -= 2;
                    break;
                case 0x29:
                    c8->I = c8->registers[d_op.x];
                    break;
                case 0x33:
                    uint8_t val = c8->registers[d_op.x];
                    c8->memory[c8->I] = (val / 100);
                    c8->memory[c8->I + 1] = (val % 100) / 10;
                    c8->memory[c8->I + 2] = val % 10;
            }
            return 0;
        default:
            fprintf(stderr, "Unknown opcode 0x%04X at 0x%04X\n", d_op.op, c8->pc - 2);
            return 1;
    }
}
