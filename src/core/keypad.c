#include "keypad.h"

#include <stddef.h>
#include <strings.h>

// clang-format off
static const uint8_t KEYPAD_COSMACVIP_KEYMAP[] = {
    0x1, 0x2, 0x3, 0xC,
    0x4, 0x5, 0x6, 0xD,
    0x7, 0x8, 0x9, 0xE,
    0xA, 0x0, 0xB, 0xF,
};

static const uint8_t KEYPAD_DIGITRAN_KEYMAP[] = {
    0x0, 0x1, 0x2, 0x3,
    0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB,
    0xC, 0xD, 0xE, 0xF,
};

static const uint8_t KEYPAD_ORIGINAL_KEYMAP[] = {
    0xC, 0xD, 0xE, 0xF,
    0x8, 0x9, 0xA, 0xB,
    0x4, 0x5, 0x6, 0x7,
    0x0, 0x1, 0x2, 0x3,
};
// clang-format on

static const keypad_data_t KEYPAD_TABLE[KEYPAD_COUNT] = {
    [KEYPAD_COSMACVIP] = {"Cosmac VIP", KEYPAD_COSMACVIP_KEYMAP},
    [KEYPAD_DIGITRAN]  = {"Digitran", KEYPAD_DIGITRAN_KEYMAP},
    [KEYPAD_ORIGINAL]  = {"Original", KEYPAD_ORIGINAL_KEYMAP},
};

keypad_data_t keypad_get(keypad_type_t type) {
    if (type >= KEYPAD_COUNT) {
        keypad_data_t invalid = {NULL, NULL};
        return invalid;
    }
    return KEYPAD_TABLE[type];
}

keypad_type_t keypad_by_name(const char *name) {
    if (name == NULL) return KEYPAD_COUNT;

    for (uint8_t i = 0; i < KEYPAD_COUNT; ++i) {
        if (strcasecmp(name, KEYPAD_TABLE[i].name) == 0) {
            return (keypad_type_t)i;
        }
    }

    return KEYPAD_COUNT;
}

uint16_t keypad_map_from_input(uint16_t state, keypad_type_t type) {
    keypad_data_t data = keypad_get(type);
    uint16_t      keypad;

    for (uint8_t i = 0x0; i <= 0xF; ++i) {
        uint8_t key_state  = (state >> i) & 0x1;
        uint8_t output_bit = data.keymap[i];
        keypad |= key_state << output_bit;
    }

    return keypad;
}
