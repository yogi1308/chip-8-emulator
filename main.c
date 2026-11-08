#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 4096
#define PC_START_ADDRESS 0x200
#define FONT_SET_START_ADDRESS 0x50
#define FONTSET_SIZE 80

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
            break;
        }
        memory[PC_START_ADDRESS + index] = c;
        index++;
    }
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    uint16_t pc = PC_START_ADDRESS;
    uint16_t opcode;
    uint8_t memory[MEMORY_SIZE] = {0};

    load_fontset(memory);
    if (load_rom(memory, argc, argv) == 1) {
        return 1;
    }

    // print memory
    for (int i = 0; i < MEMORY_SIZE; i++) {
        printf("0x%02X\n", memory[i]);
    }
    return 0;
}
