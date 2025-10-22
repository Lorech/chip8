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
    const char    *name;
    const uint8_t *data;
    uint16_t       size;
} font_data_t;

/**
 * Get the data for a specific font.
 *
 * @param type - The type of the font to retrieve data for
 * @returns The data for the requested font or an empty object if invalid
 */
font_data_t font_get(font_type_t type);

/**
 * Gets the type of font by its case insensitive name.
 *
 * @param name - The name of the font to search for
 * @returns The type of the font, or FONT_COUNT if invalid
 */
font_type_t font_by_name(const char *name);
