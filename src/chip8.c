#include "chip8.h"

#include <string.h>

#include "bitmask.h"
#include "font.h"
#include "log.h"

void chip8_init(chip8_t *chip8) {
    memset(chip8, 0, sizeof(chip8_t));
    chip8->pc            = PROGRAM_START;
    chip8->stack_pointer = -1;
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
        .status             = CHIP8_OK,
        .opcode             = 0,
        .frame_buffer_dirty = false,
        .flag_set           = false,
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
                result->status = CHIP8_STACK_EMPTY;
                return false;
            }
        case 0x6: // Set Variable
            chip8->v[N2(result->opcode)] = B2(result->opcode);
            return true;
        case 0x7: // Add to Variable
            chip8->v[N2(result->opcode)] += B2(result->opcode);
            return true;
        case 0xA: // Set Index to Address
            chip8->i = MA(result->opcode);
            return true;
        case 0xD: // Draw
            return chip8_execute_draw_instruction(chip8, result);
        default:
            // TODO: Change to CHIP8_INSTRUCTION_INVALID
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }

    return true;
}

static bool chip8_execute_system_instruction(chip8_t *chip8, chip8_state_t *result) {
    switch (result->opcode) {
        case 0x00E0: // Clear Screen
            memset(chip8->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
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

static bool chip8_execute_draw_instruction(chip8_t *chip8, chip8_state_t *result) {
    // Starting positions for drawing, which wrap across the screen
    uint8_t x = chip8->v[N2(result->opcode)] & (DISPLAY_WIDTH - 1);
    uint8_t y = chip8->v[N3(result->opcode)] & (DISPLAY_HEIGHT - 1);

    // Data for drawing the actual sprite
    uint8_t  h      = N4(result->opcode);
    uint8_t *sprite = &chip8->memory[chip8->i];

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
            if (*old && p) result->flag_set = true;
            *old ^= p;
        }
    }

    result->frame_buffer_dirty = true;
    chip8->v[0xF]              = result->flag_set;

    return true;
}
