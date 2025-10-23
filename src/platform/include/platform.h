#pragma once

#include <stdint.h>

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
 * Draws a new frame buffer on the screen.
 *
 * @param buffer - The frame buffer to display
 * @param width - The width of the display
 * @param height - The height of the display
 */
void platform_draw_display(uint8_t *buffer, uint8_t width, uint8_t height);

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
