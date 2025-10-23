#include "chip8.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bitmask.h"
#include "font.h"
#include "log.h"

void chip8_init(chip8_t *chip8, uint32_t rng_seed) {
    memset(chip8, 0, sizeof(chip8_t));
    chip8->pc            = PROGRAM_START;
    chip8->stack_pointer = -1;
    chip8_load_font(chip8, DEFAULT_FONT);
    srand(rng_seed); // TODO: Use platform RNG
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
    chip8_init(chip8, chip8->rng_seed); // TODO: Use platform RNG
    chip8_load_font(chip8, existing_font);
    memcpy(&chip8->memory[PROGRAM_START], program, size);
    return true;
}

chip8_state_t chip8_run_cycle(chip8_t *chip8) {
    chip8_state_t result = {
        .status             = CHIP8_OK,
        .opcode             = 0,
        .frame_buffer_dirty = false,
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
        case 0x0: // System Call
            return chip8_execute_system_instruction(chip8, result);
        case 0x1: // Jump to Address
            chip8->pc = MA(result->opcode);
            return true;
        case 0x2: // Execute Subroutine
            if (chip8->stack_pointer < STACK_SIZE - 1) {
                chip8->stack[++chip8->stack_pointer] = chip8->pc;
                chip8->pc                            = MA(result->opcode);
                return true;
            } else {
                result->status = CHIP8_STACK_FULL;
                return false;
            }
        case 0x3: // Skip if Variable Equals
            if (chip8->v[N2(result->opcode)] == B2(result->opcode)) {
                chip8->pc += 2;
            }
            return true;
        case 0x4: // Skip if Variable Not Equals
            if (chip8->v[N2(result->opcode)] != B2(result->opcode)) {
                chip8->pc += 2;
            }
            return true;
        case 0x5: // Skip if Variables Equal
            if (chip8->v[N2(result->opcode)] == chip8->v[N3(result->opcode)]) {
                chip8->pc += 2;
            }
            // N4 is unused and can contain any value
            return true;
        case 0x6: // Set Variable
            chip8->v[N2(result->opcode)] = B2(result->opcode);
            return true;
        case 0x7: // Add to Variable
            chip8->v[N2(result->opcode)] += B2(result->opcode);
            return true;
        case 0x8: // Arithmetic & Logic
            return chip8_execute_arithmetic_instruction(chip8, result);
        case 0x9: // Skip if Variables Not Equal
            if (chip8->v[N2(result->opcode)] != chip8->v[N3(result->opcode)]) {
                chip8->pc += 2;
            }
            // N4 is unused and can contain any value
            return true;
        case 0xA: // Set Index to Address
            chip8->i = MA(result->opcode);
            return true;
        case 0xB: // Jump with Offset
            // clang-format off
            // TODO: Make this behavior configurable at runtime.
#ifdef LEGACY_OFFSET_JUMP_BEHAVIOR
            chip8->pc = MA(result->opcode) + chip8->v[0];
#else
            chip8->pc = MA(result->opcode) + chip8->v[N2(result->opcode)];
#endif
            return true;
            // clang-format on
        case 0xC: // RNG
            // TODO: Use platform RNG
            chip8->v[N2(result->opcode)] = rand() & B2(result->opcode);
            return true;
        case 0xD: // Draw
            return chip8_execute_draw_instruction(chip8, result);
        case 0xE: // Skip if Key
            return chip8_execute_keypress_instruction(chip8, result);
        case 0xF: // Miscellaneous
            return chip8_execute_misc_instruction(chip8, result);
        default:
            // Should be unreachable, but added for completion sake
            result->status = CHIP8_INSTRUCTION_INVALID;
            return false;
    }

    return true;
}

static bool chip8_execute_system_instruction(chip8_t *chip8, chip8_state_t *result) {
    switch (result->opcode) {
        case 0x00E0: // Clear Screen
            memset(chip8->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
            result->frame_buffer_dirty = true;
            return true;
        case 0x00EE: // Return from Subroutine
            if (chip8->stack_pointer >= 0) {
                chip8->pc = chip8->stack[chip8->stack_pointer--];
                return true;
            } else {
                result->status = CHIP8_STACK_EMPTY;
                return false;
            }
        default:
            // All other opcodes execute native machine code at address 0xNNN
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }
}

static bool chip8_execute_arithmetic_instruction(chip8_t *chip8, chip8_state_t *result) {
    uint8_t *x = &chip8->v[N2(result->opcode)];
    uint8_t *y = &chip8->v[N3(result->opcode)];
    uint8_t *f = &chip8->v[0xF];

    switch (N4(result->opcode)) {
        case 0x0: // Set
            *x = *y;
            return true;
        case 0x1: // Bitwise OR
            *x |= *y;
            return true;
        case 0x2: // Bitwise AND
            *x &= *y;
            return true;
        case 0x3: // Bitwise XOR
            *x ^= *y;
            return true;
        case 0x4: // Add with Carry
            if (*x > UINT8_MAX - *y) *f = 0x1;
            *x += *y;
            return true;
        case 0x5: // Subtract X from Y
            if (*x > *y) *f = 0x1;
            *x = *x - *y;
            return true;
        case 0x6: // Shift Right
            // clang-format off
            // TODO: Make this behavior configurable at runtime.
#ifdef LEGACY_SHIFT_BEHAVIOR
            *x = *y;
#endif
            *f = (*x) & 0x1;
            *x >>= 0x1;
            return true;
            // clang-format on
        case 0x7: // Subtract Y from X
            if (*y > *x) *f = 0x1;
            *x = *y - *x;
            return true;
        case 0xE: // Shift Left
            // clang-format off
            // TODO: Make this behavior configurable at runtime.
#ifdef LEGACY_SHIFT_BEHAVIOR
            *x = *y;
#endif
            *f = (*x >> 7) & 0x1;
            *x <<= 0x1;
            return true;
            // clang-format on
        default:
            // Remaining instructions do not resolve
            result->status = CHIP8_INSTRUCTION_INVALID;
            return false;
    }
}

static bool chip8_execute_draw_instruction(chip8_t *chip8, chip8_state_t *result) {
    // Starting positions for drawing, which wrap across the screen
    uint8_t x = chip8->v[N2(result->opcode)] & (DISPLAY_WIDTH - 1);
    uint8_t y = chip8->v[N3(result->opcode)] & (DISPLAY_HEIGHT - 1);

    // Data for drawing the actual sprite
    uint8_t  h      = N4(result->opcode);
    uint8_t *sprite = &chip8->memory[chip8->i];
    uint8_t *f      = &chip8->v[0xF]; // Flag gets set if a pixel turns off

    // Iterate sprite byte-by-byte
    for (uint8_t j = 0; j < h; ++j) {
        uint8_t row = sprite[j];
        // Iterate pixels bit-by-bit
        for (uint8_t i = 0; i < 8; ++i) {
            // Sprites do not wrap across the screen
            if (x + i >= DISPLAY_WIDTH || y + j >= DISPLAY_HEIGHT) continue;
            // Draw left-to-right (as opposed to 1 << i)
            bool  p   = row & (0x80 >> i);
            bool *old = &chip8->display[(y + j) * DISPLAY_WIDTH + (x + i)];
            if (*old && p) *f = 0x1;
            *old ^= p;
        }
    }

    result->frame_buffer_dirty = true;
    return true;
}

static bool chip8_execute_keypress_instruction(chip8_t *chip8, chip8_state_t *result) {
    uint8_t *x = &chip8->v[N2(result->opcode)];

    switch (B2(result->opcode)) {
        case 0x9E:
        case 0xA1:
            // TODO: Implement when keypress handling has been added
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
        default:
            // Remaining instructions do not resolve
            result->status = CHIP8_INSTRUCTION_INVALID;
            return false;
    }
}

static bool chip8_execute_misc_instruction(chip8_t *chip8, chip8_state_t *result) {
    uint8_t *x = &chip8->v[N2(result->opcode)];

    switch (B2(result->opcode)) {
        case 0x07: // Set to Delay Timer
            *x = chip8->delay_timer;
            return true;
        case 0x0A: // Get Key
            // TODO: Implement when keypress handling has been added
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
        case 0x15: // Set Delay Timer
            chip8->delay_timer = *x;
            return true;
        case 0x18: // Set Sound Timer
            chip8->sound_timer = *x;
            return true;
        case 0x1E: // Add to Index
            chip8->i += *x;
            return true;
        case 0x29: // Get Character
            chip8->i = FONT_START + 5 * ((*x) & 0xF);
            return true;
        case 0x33: // Decimal Conversion
            chip8->memory[chip8->i + 0] = *x / 100 % 10;
            chip8->memory[chip8->i + 1] = *x / 10 % 10;
            chip8->memory[chip8->i + 2] = *x / 1 % 10;
            return true;
        case 0x55: // Store Memory
            for (uint8_t j = 0; j <= N2(result->opcode); ++j) {
                chip8->memory[chip8->i + j] = chip8->v[j];
            }
            // TODO: Make this configurable at runtime
#ifdef LEGACY_MEMORY_BEHAVIOR
            chip8->i += N2(result->opcode) + 1;
#endif
            return true;
        case 0x65: // Load Memory
            for (uint8_t j = 0; j <= N2(result->opcode); ++j) {
                chip8->v[j] = chip8->memory[chip8->i + j];
            }
            // TODO: Make this configurable at runtime
#ifdef LEGACY_MEMORY_BEHAVIOR
            chip8->i += N2(result->opcode) + 1;
#endif
            return true;
        default:
            // Remaining instructions do not resolve
            result->status = CHIP8_INSTRUCTION_INVALID;
            return false;
    }
}
