#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"
#include "headers/dex.h"
#include "headers/loaders.h"
#include "raylib.h"

#define SCALE 10

int main(int argc, char *argv[]) {
    srand(time(NULL));
    chip8 c8 = init_chip8();
    if (load_flags(&c8, argc, argv) == 1) {
        return 1;
    }
    // TODO add strcmp to compare the flags passed
    load_fontset(c8.memory);
    if (load_rom(c8.memory, argc, argv) == 1) {
        return 1;
    }

    InitWindow(64 * SCALE, 32 * SCALE, "Chip-8 Emulator");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // Update variables here
        c8.delay_timer--;
        c8.sound_timer--;
        printf("0x%02X\n", c8.pc);
        c8.opcode = fetch(c8.memory, c8.pc);
        c8.pc += 2;
        decoded_opcode d_op = decode(c8.opcode);
        print_decoded_opcode(d_op, c8.opcode);
        if (execute(d_op, &c8) != 0) {
            return 1;
        }
        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                if (c8.screen[y][x] == 1) {
                    DrawRectangle(x * SCALE, y * SCALE, 10, 10, BLUE);
                }
            }
        }

        EndDrawing();
    }

    // De-initialize and close window
    CloseWindow();

    // print memory
    /* for (int i = 0; i < MEMORY_SIZE; i++) { */
    /*     printf("0x%02X %d\n", memory[i], i); */
    /* } */
    return 0;
}
