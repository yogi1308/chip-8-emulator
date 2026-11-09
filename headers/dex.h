#pragma once

#include <stdint.h>
#include "../chip8.h"

uint16_t fetch(uint8_t *memory, uint16_t pc);

void print_decoded_opcode(decoded_opcode d_op, uint16_t opcode);

decoded_opcode decode(uint16_t opcode);

int execute(decoded_opcode d_op, chip8 *c8);
