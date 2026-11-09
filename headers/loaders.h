#pragma once
#include <stdint.h>
#include "../chip8.h"

void load_fontset(uint8_t *memory);

int load_rom(uint8_t *memory, int argc, char *argv[]);
int load_flags(chip8 *c8, int argc, char *argv[]);
