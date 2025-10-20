#include <stdio.h>

#include "cpu.h"
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

    cpu_t       cpu;
    memory_t    memory;
    chipstack_t stack;
    display_t   display;
    timer_t     timer;

    memory  = memory_create();
    stack   = stack_create(4);
    display = display_create();

    timer_init(&timer);
    cpu_init(&cpu, &memory, &stack, &timer, &display);
    cpu_load_program(&cpu, program, sizeof(program));

    SetTargetFPS(FRAMES_PER_SECOND);
    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "CHIP-8");

    while (!WindowShouldClose()) {
        BeginDrawing();

        // TODO: Don't rely on FPS for CPU cycles.
        cpu_state_t state = cpu_run_cycle(&cpu);
        for (uint8_t x = 0; x < DISPLAY_WIDTH; ++x) {
            for (uint8_t y = 0; y < DISPLAY_HEIGHT; ++y) {
                if (cpu.display->screen[y * DISPLAY_WIDTH + x]) {
                    DrawPixel(x, y, RAYWHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
