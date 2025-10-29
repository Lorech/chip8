#pragma once

#include <stdint.h>

#ifndef DEFAULT_KEYPAD
#define DEFAULT_KEYPAD KEYPAD_COSMACVIP
#endif

typedef enum {
    KEYPAD_COSMACVIP,
    KEYPAD_DIGITRAN,
    KEYPAD_ORIGINAL,
    KEYPAD_COUNT
} keypad_type_t;

typedef struct {
    const char    *name;
    const uint8_t *keymap;
} keypad_data_t;

/**
 * Get keypad information for a specific keypad.
 *
 * @param type - The type of the keypad to retrieve the information for
 * @returns The data for the requested keypad or an empty object if invalid
 */
keypad_data_t keypad_get(keypad_type_t type);

/**
 * Gets the type of keypad by its case insensitive name.
 *
 * @param name - The name of the keypad to search for
 * @returns The type of the keypad, or KEYPAD_COUNT if invalid
 */
keypad_type_t keypad_by_name(const char *name);

/**
 * Remaps a generic keypad into a specific keypad.
 *
 * The generic keypad is considered one that is a consecutively arranged keypad,
 * spanning from 0 in the top-left corner to F in the bottom-right corner. This
 * function takes this data, and shifts all the bits around to convert it into
 * an equivalent data structure, but tailored to the layout of a specific keypad
 * type.
 *
 * For example, given a requested keypad type that spans from F to 0, the output
 * state would be equal to the input state if all bits were shifted left by 0xF.
 *
 * @param state - The generic state of the keypad
 * @param type - The keypad type to map the state to
 * @returns The keypad state as per the layout of the requested type
 */
uint16_t keypad_map_from_input(uint16_t state, keypad_type_t type);
