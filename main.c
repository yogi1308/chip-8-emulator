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

void check_keyboard(chip8 *c8) {
    const char *map = c8->hex_keys ? "0123456789ABCDEF" : "X123QWEASDZC4RFV";
    for (int k = 0; k < 16; k++) {
        c8->keyboard[k] = IsKeyDown(map[k]);
    }
}

void cleanup(Sound *sound) {
    UnloadSound(*sound);
    CloseAudioDevice();
    CloseWindow();
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    chip8 c8 = init_chip8();
    if (load_flags(&c8, argc, argv) == 1) {
        return 1;
    }
    load_fontset(c8.memory);
    if (load_rom(c8.memory, argc, argv) == 1) {
        return 1;
    }

    InitWindow(SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, "Chip-8 Emulator");
    SetTargetFPS(60);
    InitAudioDevice();
    Sound beep = LoadSound("beep.wav");
    while (!WindowShouldClose()) {
        // Update variables here
        /* printf("0x%02X\n", c8.pc); */
        if (c8.sound_timer > 0) {
            if (!IsSoundPlaying(beep)) {
                PlaySound(beep);
            }
        } else {
            if (IsSoundPlaying(beep)) {
                StopSound(beep);
            }
        }
        check_keyboard(&c8);
        for (int i = 0; i < c8.speed; i++) {
            if (c8.pc > MEMORY_SIZE - 2) {
                fprintf(stderr, "PC out of bounds: 0x%04X\n", c8.pc);
                cleanup(&beep);
                return 1;
            }
            c8.opcode = fetch(c8.memory, c8.pc);
            c8.pc += 2;
            decoded_opcode d_op = decode(c8.opcode);
            /* print_decoded_opcode(d_op, c8.opcode); */
            if (execute(d_op, &c8) != 0) {
                cleanup(&beep);
                return 1;
            }
        }
        if (c8.delay_timer > 0)
            c8.delay_timer--;
        if (c8.sound_timer > 0)
            c8.sound_timer--;

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (c8.screen[y][x] == 1) {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, BLUE);
                }
            }
        }
        EndDrawing();
    }

    // De-initialize and cleanup window
    cleanup(&beep);

    return 0;
}
