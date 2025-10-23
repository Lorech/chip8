#include <stdint.h>
#include <stdio.h>

#include "chip8.h"
#include "platform.h"

int main(int argc, char **argv) {
    // TODO: Move ROM loading to platform layer
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

    uint64_t seed = platform_get_time();
    platform_seed_rng(seed);
    platform_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, FRAMES_PER_SECOND);

    chip8_t chip8;
    chip8_init(&chip8, seed); // TODO: Use platform RNG
    chip8_load_program(&chip8, program, sizeof(program));

    double cpu_tick   = 1.0 / INSTRUCTIONS_PER_SECOND;
    double clock_tick = 1.0 / 1.0;

    double time             = platform_get_time();
    double next_cpu_cycle   = time;
    double next_clock_cycle = time;

    // TODO: Figure out how to prevent this from exploding the CPU cross-platform
    do {
        double time = platform_get_time();

        if (time > next_cpu_cycle) {
            chip8_state_t state = chip8_run_cycle(&chip8);
            if (state.frame_buffer_dirty) platform_draw_display(chip8.display);
            if (state.sound_timer_set) {
                chip8.playing_sound = true;
                platform_play_audio();
            }
            next_cpu_cycle += cpu_tick;
        }

        if (time > next_clock_cycle) {
            if (chip8.sound_timer > 0) chip8.sound_timer -= 1;
            if (chip8.delay_timer > 0) chip8.delay_timer -= 1;
            if (chip8.playing_sound && chip8.sound_timer == 0) {
                chip8.playing_sound = false;
                platform_stop_audio();
            }
            next_clock_cycle += clock_tick;
        }
    } while (true);

    platform_close();
}
