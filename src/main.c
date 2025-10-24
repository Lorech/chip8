#include <stdint.h>
#include <stdio.h>

#include "chip8.h"
#include "platform.h"

#define SECOND 1000000 // 1 second in microseconds

int main(int argc, char **argv) {
    uint64_t seed = platform_get_time();
    platform_seed_rng(seed);
    platform_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, FRAMES_PER_SECOND);

    uint8_t rom[MEMORY_SIZE - PROGRAM_START];
    bool    loaded = platform_load_rom(rom, sizeof(rom), argc, argv);
    if (!loaded) {
        printf("ERROR: Failed to load ROM.");
        return 1;
    }

    chip8_t chip8;
    chip8_init(&chip8, platform_rng);
    chip8_load_program(&chip8, rom, sizeof(rom));

    // Draw the display once to ensure it is at a stable, empty state
    platform_draw_display(chip8.display);

    uint64_t target_frame_time   = SECOND / FRAMES_PER_SECOND;
    uint64_t cpu_ticks_per_frame = INSTRUCTIONS_PER_SECOND / FRAMES_PER_SECOND;

    uint64_t last_time       = platform_get_time();
    uint64_t next_clock_tick = last_time + SECOND;

    do {
        uint64_t time       = platform_get_time();
        uint64_t frame_time = time - last_time;
        last_time           = time;
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
            next_clock_tick += SECOND;
        }
    } while (true);

    platform_close();
}
