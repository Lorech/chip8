#pragma once

#include <stdint.h>

#define FONT_START 0x50 // Memory address; general convention

typedef enum {
    FONT_CHIP48,
    FONT_COSMACVIP,
    FONT_DREAM6800,
    FONT_ETI660,
    FONT_COUNT
} font_type_t;

typedef struct {
    const uint8_t *data;
    uint16_t       size;
} font_data_t;

/**
 * Get the data for a specific font.
 *
 * @param type - The name of the font to retrieve
 * @returns The data for the requested font or an empty object if invalid
 */
font_data_t font_get(font_type_t type);
