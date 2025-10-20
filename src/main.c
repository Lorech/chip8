#include <stdio.h>

#include "chip8.h"
#include "display.h"
#include "memory.h"
#include "raylib.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No ROM path provided! Exiting.\n");
        return 1;
    }

    char   *path = argv[1];
    FILE   *f;
    uint8_t program[MEMORY_SIZE - PROGRAM_START];

    f = fopen(path, "rb");
    fread(program, 1, MEMORY_SIZE - PROGRAM_START, f);
    fclose(f);

    chip8_t     chip8;
    memory_t    memory;
    chipstack_t stack;
    display_t   display;
    timer_t     timer;

    memory  = memory_create();
    stack   = stack_create(4);
    display = display_create();

    timer_init(&timer);
    chip8_init(&chip8, &memory, &stack, &timer, &display);
    chip8_load_program(&chip8, program, sizeof(program));

    SetTargetFPS(FRAMES_PER_SECOND);
    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "CHIP-8");

    while (!WindowShouldClose()) {
        BeginDrawing();

        // TODO: Don't rely on FPS for chip8 cycles.
        chip8_state_t state = chip8_run_cycle(&chip8);
        for (uint8_t x = 0; x < DISPLAY_WIDTH; ++x) {
            for (uint8_t y = 0; y < DISPLAY_HEIGHT; ++y) {
                if (chip8.display->screen[y * DISPLAY_WIDTH + x]) {
                    DrawPixel(x, y, RAYWHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
