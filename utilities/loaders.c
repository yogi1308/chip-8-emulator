#include "../headers/loaders.h"

#include <stdio.h>
#include <string.h>

#include "../chip8.h"
#include "../headers/font.h"

void load_fontset(uint8_t *memory) {
    for (int i = 0; i < FONTSET_SIZE; i++) { // load font into memory
        memory[FONT_SET_START_ADDRESS + i] = fontset[i];
    }
}

int load_flags(chip8 *c8, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--shift-uses-vy") == 0) {
                c8->shift_uses_vy = true;
            } 
            else if (strcmp(argv[i], "--bxnn") == 0) {
                c8->bxnn = true;
            }
            else {
                fprintf(stderr, "Unknown flag %s\n", argv[i]);
                return 1;
            }
        }
    }
    return 0;
}

int load_rom(uint8_t *memory, int argc, char *argv[]) {
    // load program into memory
    if (argc == 1) {
        fprintf(stderr, "No arguments passed. Needs the name of the file to be passed as an argument\n");
        return 1;
    }

    FILE *fp = NULL;
    char *filepath = NULL;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                fp = fopen(argv[i], "rb");
                filepath = argv[i];
                break;
            }
        }
    }
    if (fp == NULL) {
        if (filepath != NULL) {
            fprintf(stderr, "File pointer is null. Could not find %s\n", filepath);
        } else {
            fprintf(stderr, "File pointer is null. File may not exist\n");
        }
        return 1;
    }

    int c;
    int index = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (index >= (MEMORY_SIZE - PC_START_ADDRESS)) { // prevents memory overflow
            fclose(fp);
            fprintf(stderr, "Program exceeds memory\n");
            return 1;
        }
        memory[PC_START_ADDRESS + index] = c;
        index++;
    }
    fclose(fp);
    return 0;
}
