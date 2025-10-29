#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Initialize the platform's hardware state before using it.
 *
 * @param width - The width of the display
 * @param height - The height of the display
 * @param fps - The frames per second the display should target
 */
void platform_init(uint8_t width, uint8_t height, uint8_t fps);

/**
 * Clear the platform's hardware state before disabling it.
 */
void platform_close(void);

/**
 * If the program should continue execution.
 *
 * This should be used as the condition for running the main loop, waiting on
 * this function to return `false` to exit out and deinitialize everything.
 *
 * Such a scenario may be impossible for some platforms, others may rely on some
 * external callback, e.g., a window being closed to signal the program to halt.
 *
 * @returns If another frame should be executed
 */
bool platform_should_run_frame(void);

/**
 * Sleep for a fixed amount of time.
 *
 * Avoid CPU churn while running an empty main loop.
 *
 * @param microseconds - The number of microseconds to sleep for
 */
void platform_sleep(uint64_t microseconds);

/**
 * Gets the current timestamp with microsecond precision from the system.
 *
 * @returns The current timestamp in microseconds
 */
uint64_t platform_get_time(void);

/**
 * Seeds the random number generator.
 *
 * @param seed - The seed to use for the generator.
 */
void platform_seed_rng(uint64_t seed);

/**
 * Generates a random number.
 *
 * Limited to 8 bits as the CHIP-8 does not require any more.
 *
 * @returns A random number
 */
uint8_t platform_rng(void);

/**
 * Loads a ROM into the application's memory.
 *
 * The last two input arguments match to those of `main` to allow loading a
 * ROM from a file which is provided via CLI arguments when starting the
 * application.
 *
 * @param rom - The loaded ROM
 * @param max_size - The maximum size permissible for a valid ROM
 * @param argc - The number of arguments provided to `main`
 * @param argv - The arguments provided to `main`
 * @returns If the ROM was loaded successfully
 */
bool platform_load_rom(uint8_t *rom, size_t max_size, int argc, char **argv);

/**
 * Draws a new frame buffer on the screen.
 *
 * @param buffer - The frame buffer to display
 */
void platform_draw_display(bool *buffer);

/**
 * Starts playing a sound if one is not already active.
 */
void platform_play_audio(void);

/**
 * Stops playing the active sounds.
 */
void platform_stop_audio(void);

/**
 * Gets the current state of the keypad.
 *
 * Since the CHIP-8 keypad only consists of 16 keys, a single 16-bit integer
 * is returned, where each bit represents a single key being on or off in a
 * way that is agnostic to the CHIP-8 itself, as several different layouts may
 * be supported, which can be changed at runtime.
 *
 * The returned keypad bits map to the following keypad structure, numbers
 * indicating bits from least to most significant:
 *
 * -------------------------
 * | 0x0 | 0x1 | 0x2 | 0x3 |
 * -------------------------
 * | 0x4 | 0x5 | 0x6 | 0x7 |
 * -------------------------
 * | 0x8 | 0x9 | 0xA | 0xB |
 * -------------------------
 * | 0xC | 0xD | 0xE | 0xF |
 * -------------------------
 *
 * @returns The current state of the keypad
 */
uint16_t platform_get_keypad(void);
