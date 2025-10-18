#pragma once

#include <stdbool.h>

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
