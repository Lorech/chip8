#include "chip8.h"

#include <string.h>

#include "bitmask.h"
#include "font.h"
#include "log.h"

void chip8_init(chip8_t *chip8) {
    memset(chip8, 0, sizeof(chip8_t));
    chip8->pc = PROGRAM_START;
    chip8_load_font(chip8, DEFAULT_FONT);
}

bool chip8_load_font(chip8_t *chip8, font_type_t type) {
    if (type >= FONT_COUNT) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Attempted to load invalid font.");
        return false;
    }

    font_data_t font = font_get(type);
    if (!font.data) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Attempted to load empty font.");
        return false;
    } else if (MEMORY_SIZE - FONT_START < font.size) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Attempted to load oversized font.");
        return false;
    }

    memcpy(&chip8->memory[FONT_START], font.data, font.size);
    chip8->font = type;
    return true;
}

bool chip8_load_program(chip8_t *chip8, const uint8_t *program, uint16_t size) {
    if (!program) {
        LOG_WARN(LOG_SUBSYS_MEMORY, "Attempted to load empty program.");
        return false;
    } else if (MEMORY_SIZE - PROGRAM_START < size) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Attempted to load oversized program.");
        return false;
    }

    font_type_t existing_font = chip8->font;
    chip8_init(chip8);
    chip8_load_font(chip8, existing_font);
    memcpy(&chip8->memory[PROGRAM_START], program, size);
    return true;
}

chip8_state_t chip8_run_cycle(chip8_t *chip8) {
    chip8_state_t result = {
        .status = CHIP8_OK,
        .opcode = 0,
    };

    bool fetch_success = chip8_fetch_instruction(chip8, &result);
    if (!fetch_success) return result;

    bool execute_success = chip8_execute_instruction(chip8, &result);
    if (!execute_success) return result;

    return result;
}

static bool chip8_fetch_instruction(chip8_t *chip8, chip8_state_t *result) {
    uint8_t bytes[2];
    memcpy(&bytes, &chip8->memory[chip8->pc], sizeof(bytes));
    result->opcode = (bytes[0] << 8) | bytes[1];
    chip8->pc += 2;
    return true;
}

static bool chip8_execute_instruction(chip8_t *chip8, chip8_state_t *result) {
    switch (N1(result->opcode)) {
        case 0x0:
            switch (result->opcode) {
                case 0x00E0:
                    memset(chip8->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
                    return true;
                default:
                    // TODO: Change to CHIP8_INSTRUCTION_INVALID
                    result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
                    return false;
            }
        case 0x1:
            chip8->pc = MA(result->opcode);
            return true;
        case 0x6:
            chip8->v[N2(result->opcode)] = B2(result->opcode);
            return true;
        case 0x7:
            chip8->v[N2(result->opcode)] += B2(result->opcode);
            return true;
        case 0xA:
            chip8->i = MA(result->opcode);
            return true;
        case 0xD: {
            uint8_t x     = chip8->v[N2(result->opcode)];
            uint8_t y     = chip8->v[N3(result->opcode)];
            uint8_t h     = N4(result->opcode);
            chip8->v[0xF] = chip8_draw_sprite(chip8, x, y, h, &chip8->memory[chip8->i]);
            return true;
        }
        default:
            // TODO: Change to CHIP8_INSTRUCTION_INVALID
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }

    return true;
}

static bool chip8_draw_sprite(
    chip8_t *chip8,
    uint8_t  x,
    uint8_t  y,
    uint8_t  h,
    uint8_t *sprite
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
            bool *old = &chip8->display[(y + j) * DISPLAY_WIDTH + (x + i)];
            if (*old && p) vf = true;
            *old ^= p;
        }
    }

    return vf;
}
