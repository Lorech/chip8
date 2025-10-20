#pragma once

#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH     64
#define DISPLAY_HEIGHT    32
#define FRAMES_PER_SECOND 60

typedef struct {
    bool screen[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} display_t;

/**
 * Initializes the display module.
 *
 * Due to the very small and fixed memory size required by the CHIP-8, memory
 * can be allocated on the stack, so there is no corresponding cleanup function.
 *
 * @returns The created display module
 */
display_t display_create(void);

/**
 * Draws a sprite to the display.
 *
 * Corresponds to the 0xDxxx function of the CHIP-8. The coordinate values
 * must be given in terms of actual value instead of variable indices.
 *
 * @param display - The display to draw to
 * @param x - The x coordinate to start drawing from
 * @param y - The y coordinate to start drawing from
 * @param size - The size of the sprite (should match the size of *sprite)
 * @param sprite - The sprite data to draw
 * @returns The new value of the flag register after drawing
 */
bool display_draw_sprite(
    display_t *display,
    uint8_t    x,
    uint8_t    y,
    uint8_t    h,
    uint8_t   *sprite
);
