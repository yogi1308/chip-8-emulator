#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"
#include "headers/dex.h"
#include "headers/loaders.h"
#include "raylib.h"

#define SCALE 10

void check_key_pressed(chip8 *c8) {
    const char *keys = "X123QWEASDZC4RFV";
    if (c8->hex_keys) {
        keys = "0123456789ABCDEF";
    }
    c8->key_pressed = -1;
    for (int k = 0; k < 16; k++) {
        if (IsKeyDown(keys[k])) {
            c8->key_pressed = k;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    chip8 c8 = init_chip8();
    if (load_flags(&c8, argc, argv) == 1) {
        CloseWindow();
        return 1;
    }
    load_fontset(c8.memory);
    if (load_rom(c8.memory, argc, argv) == 1) {
        CloseWindow();
        return 1;
    }

    InitWindow(64 * SCALE, 32 * SCALE, "Chip-8 Emulator");
    SetTargetFPS(800);
    while (!WindowShouldClose()) {
        // Update variables here
        if (c8.delay_timer > 0) {
            c8.delay_timer--;
        }

        if (c8.sound_timer > 0) {
            c8.sound_timer--;
        }
        /* printf("0x%02X\n", c8.pc); */
        c8.opcode = fetch(c8.memory, c8.pc);
        c8.pc += 2;
        decoded_opcode d_op = decode(c8.opcode);
        /* print_decoded_opcode(d_op, c8.opcode); */
        check_key_pressed(&c8);
        if (execute(d_op, &c8) != 0) {
            CloseWindow();
            return 1;
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                if (c8.screen[y][x] == 1) {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, BLUE);
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
