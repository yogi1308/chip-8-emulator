#include "../chip8.h"
#include "../headers/loaders.h"
#include "../headers/font.h"
#include <stdio.h>
void load_fontset(uint8_t *memory) {
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


