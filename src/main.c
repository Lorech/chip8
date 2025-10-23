#include <stdint.h>
#include <stdio.h>

#include "chip8.h"
#include "platform.h"

int main(int argc, char **argv) {
    uint64_t seed = platform_get_time();
    platform_seed_rng(seed);
    platform_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, FRAMES_PER_SECOND);

    uint16_t max_rom_size = MEMORY_SIZE - PROGRAM_START;
    uint8_t  rom[max_rom_size];
    bool     loaded = platform_load_rom(rom, max_rom_size, argc, argv);
    if (!loaded) {
        printf("ERROR: Failed to load ROM.");
        return 1;
    }

    chip8_t chip8;
    chip8_init(&chip8, platform_rng);
    chip8_load_program(&chip8, rom, sizeof(rom));

    double target_frame_time   = 1.0 / FRAMES_PER_SECOND;
    double cpu_ticks_per_frame = (double)INSTRUCTIONS_PER_SECOND / FRAMES_PER_SECOND;

    double last_time       = platform_get_time();
    double next_clock_tick = last_time;

    do {
        double time       = platform_get_time();
        double frame_time = time - last_time;
        last_time         = time;
        if (frame_time < target_frame_time) {
            platform_sleep(target_frame_time - frame_time);
        }

        // CPU advances by x amount of instructions each frame
        for (uint8_t i = 0; i < cpu_ticks_per_frame; ++i) {
            chip8_state_t state = chip8_run_cycle(&chip8);
            if (state.frame_buffer_dirty) platform_draw_display(chip8.display);
            if (state.sound_timer_set) {
                chip8.playing_sound = true;
                platform_play_audio();
            }
        }

        // Clocks tick once every second
        if (time > next_clock_tick) {
            if (chip8.sound_timer > 0) chip8.sound_timer -= 1;
            if (chip8.delay_timer > 0) chip8.delay_timer -= 1;
            if (chip8.playing_sound && chip8.sound_timer == 0) {
                chip8.playing_sound = false;
                platform_stop_audio();
            }
            next_clock_tick += 1.0;
        }
    } while (true);

    platform_close();
}
