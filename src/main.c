#include <stdint.h>
#include <stdio.h>

#include "chip8.h"
#include "log.h"
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

    uint64_t target_frame_time     = SECOND / FRAMES_PER_SECOND;
    uint64_t cpu_ticks_per_frame   = INSTRUCTIONS_PER_SECOND / FRAMES_PER_SECOND;
    uint64_t timer_ticks_per_frame = TICKS_PER_SECOND / FRAMES_PER_SECOND;

    uint64_t last_time = platform_get_time();
    uint64_t frame     = 0;

    do {
        uint64_t start_time         = platform_get_time();
        bool     frame_buffer_dirty = false;
        ++frame;

        chip8.keypad_state = platform_get_keypad();
        LOG_DEBUG(LOG_SUBSYS_SYSTEM, "Keypad State %u", chip8.keypad_state);
        for (uint64_t i = 0; i < cpu_ticks_per_frame; ++i) {
            chip8_state_t state = chip8_run_cycle(&chip8);
            LOG_DEBUG(LOG_SUBSYS_CPU, "Frame %u executed instruction %4X", frame, state.opcode);
            if (state.status != CHIP8_OK) LOG_WARN(LOG_SUBSYS_CPU, "Bad cycle status %d after instruction %4X.", state.status, state.opcode);
            if (state.frame_buffer_dirty) frame_buffer_dirty = true;
            if (state.sound_timer_set) {
                LOG_DEBUG(LOG_SUBSYS_TIMER, "Starting sound timer.");
                chip8.playing_sound = true;
                platform_play_audio();
            }
        }

        for (uint64_t i = 0; i < timer_ticks_per_frame; ++i) {
            if (chip8.sound_timer > 0) chip8.sound_timer -= 1;
            if (chip8.delay_timer > 0) chip8.delay_timer -= 1;
            if (chip8.playing_sound && chip8.sound_timer == 0) {
                LOG_DEBUG(LOG_SUBSYS_TIMER, "Stopping sound timer.");
                chip8.playing_sound = false;
                platform_stop_audio();
            }
        }

        if (frame_buffer_dirty) platform_draw_display(chip8.display);

        uint64_t end_time   = platform_get_time();
        uint64_t frame_time = end_time - start_time;
        if (frame_time < target_frame_time) {
            platform_sleep(target_frame_time - frame_time);
        }

        last_time = end_time;
    } while (true);

    platform_close();
}
