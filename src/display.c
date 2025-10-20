#include "display.h"

display_t display_create(void) {
    display_t display = {0};
    return display;
}

bool display_draw_sprite(
    display_t *display,
    uint8_t    x,
    uint8_t    y,
    uint8_t    h,
    uint8_t   *sprite
) {
    // Clamp the starting position within the screen.
    x = x & (DISPLAY_WIDTH - 1);
    y = y & (DISPLAY_HEIGHT - 1);

    bool vf = false;

    // Draw byte-by-byte.
    for (uint8_t j = 0; j < h; ++j) {
        uint8_t row = sprite[j];
        for (uint8_t i = 0; i < 8; ++i) {
            // Sprites do not wrap within the screen.
            if (x + i >= DISPLAY_WIDTH || y + j >= DISPLAY_HEIGHT) continue;
            // Draw left-to-right (as opposed to 1 << i)
            bool  p   = row & (0x80 >> i);
            bool *old = &display->screen[(y + j) * DISPLAY_WIDTH + (x + i)];
            if (*old && p) vf = true;
            *old ^= p;
        }
    }

    return vf;
}
