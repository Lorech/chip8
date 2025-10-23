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
 * Sleep for a fixed amount of time.
 *
 * Avoid CPU churn while running an empty main loop.
 *
 * @param seconds - The number of seconds to sleep for
 */
void platform_sleep(double seconds);

/**
 * Gets the current timestamp from the system's clock.
 *
 * @returns The current timestamp
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
 * Get the current state of the keypad.
 *
 * Since the CHIP-8 has only 16 inputs, a single 16-bit number is returned,
 * with each bit indicating if the key is pressed or not. The bits are arranged
 * from 16 (MSB) to 0 (LSB).
 *
 * @returns The current state of the keypad
 */
uint16_t platform_get_keypad(void);
