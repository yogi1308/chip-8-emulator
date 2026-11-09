#pragma once
#include <stdint.h>
#include <stdbool.h>
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
    int screen[32][64];
    int delay_timer;
    int sound_timer;
    uint16_t stack[16];
    int stack_pointer;
    bool shift_uses_vy;
    bool bxnn;
    bool hex_keys;
    int key_pressed;
    bool inc_i;
} chip8;

static inline chip8 init_chip8(void) {
    chip8 c8 = {0};
    c8.pc = PC_START_ADDRESS;
    c8.key_pressed = -1;
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
