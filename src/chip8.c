#include "chip8.h"

#include <string.h>

#include "bitmask.h"
#include "display.h"
#include "log.h"
#include "memory.h"

void chip8_init(
    chip8_t     *chip8,
    memory_t    *memory,
    chipstack_t *stack,
    timer_t     *timer,
    display_t   *display
) {
    chip8->PC      = PROGRAM_START;
    chip8->I       = 0;
    chip8->memory  = memory;
    chip8->stack   = stack;
    chip8->timer   = timer;
    chip8->display = display;
    for (uint8_t i = 0; i < 16; ++i) {
        chip8->V[i] = 0;
    }
}

void chip8_reset(chip8_t *chip8) {
    chip8->PC = PROGRAM_START;
    chip8->I  = 0;
    memset(chip8->V, 0, sizeof(chip8->V));
}

bool chip8_load_font(chip8_t *chip8, font_type_t type) {
    font_data_t font = font_get(type);
    if (!font.data) return false;
    bool success = memory_write_bytes(chip8->memory, FONT_START, font.data, font.size);
    if (success) {
        chip8->font = type;
    }
    return success;
}

bool chip8_load_program(chip8_t *chip8, const uint8_t *program, uint16_t size) {
    if (!program) {
        LOG_WARN(LOG_SUBSYS_chip8, "Attempted to load empty program.");
        return false;
    }

    bool success = memory_write_bytes(chip8->memory, PROGRAM_START, program, size);
    if (!success) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Program too large to load into memory.");
        return false;
    }

    chip8_reset(chip8);
    return true;
}

chip8_state_t chip8_run_cycle(chip8_t *chip8) {
    chip8_state_t result = {
        .status = CHIP8_OK,
        .PC     = chip8->PC,
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
    bool    success = memory_read_bytes(chip8->memory, chip8->PC, bytes, sizeof(bytes));
    if (!success) {
        result->status = CHIP8_FETCH_FAILED;
        return false;
    }

    result->opcode = (bytes[0] << 8) | bytes[1];
    chip8->PC += 2;
    return true;
}

static bool chip8_execute_instruction(chip8_t *chip8, chip8_state_t *result) {
    switch (N1(result->opcode)) {
        case 0x0:
            switch (result->opcode) {
                case 0x00E0:
                    *chip8->display = display_create();
                    return true;
                default:
                    // TODO: Change to CHIP8_INSTRUCTION_INVALID
                    result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
                    return false;
            }
        case 0x1:
            chip8->PC = MA(result->opcode);
            return true;
        case 0x6:
            chip8->V[N2(result->opcode)] = B2(result->opcode);
            return true;
        case 0x7:
            chip8->V[N2(result->opcode)] += B2(result->opcode);
            return true;
        case 0xA:
            chip8->I = MA(result->opcode);
            return true;
        case 0xD: {
            uint8_t x     = chip8->V[N2(result->opcode)];
            uint8_t y     = chip8->V[N3(result->opcode)];
            uint8_t h     = N4(result->opcode);
            chip8->V[0xF] = display_draw_sprite(chip8->display, x, y, h, &chip8->memory->data[chip8->I]);
            return true;
        }
        default:
            // TODO: Change to CHIP8_INSTRUCTION_INVALID
            result->status = CHIP8_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }

    return true;
}
