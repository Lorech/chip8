#include "cpu.h"

#include <string.h>

#include "display.h"
#include "log.h"
#include "memory.h"

void cpu_init(
    cpu_t       *cpu,
    memory_t    *memory,
    chipstack_t *stack,
    timer_t     *timer,
    display_t   *display
) {
    cpu->PC      = PROGRAM_START;
    cpu->I       = 0;
    cpu->memory  = memory;
    cpu->stack   = stack;
    cpu->timer   = timer;
    cpu->display = display;
    for (uint8_t i = 0; i < 16; ++i) {
        cpu->V[i] = 0;
    }
}

void cpu_reset(cpu_t *cpu) {
    cpu->PC = PROGRAM_START;
    cpu->I  = 0;
    memset(cpu->V, 0, sizeof(cpu->V));
}

bool cpu_load_font(cpu_t *cpu, font_type_t type) {
    font_data_t font = font_get(type);
    if (!font.data) return false;
    bool success = memory_write_bytes(cpu->memory, FONT_START, font.data, font.size);
    if (success) {
        cpu->font = type;
    }
    return success;
}

bool cpu_load_program(cpu_t *cpu, const uint8_t *program, uint16_t size) {
    if (!program) {
        LOG_WARN(LOG_SUBSYS_CPU, "Attempted to load empty program.");
        return false;
    }

    bool success = memory_write_bytes(cpu->memory, PROGRAM_START, program, size);
    if (!success) {
        LOG_ERROR(LOG_SUBSYS_MEMORY, "Program too large to load into memory.");
        return false;
    }

    cpu_reset(cpu);
    return true;
}

cpu_state_t cpu_run_cycle(cpu_t *cpu) {
    cpu_state_t result = {
        .status = CPU_OK,
        .PC     = cpu->PC,
        .opcode = 0,
    };

    bool fetch_success = cpu_fetch_instruction(cpu, &result);
    if (!fetch_success) return result;

    bool execute_success = cpu_execute_instruction(cpu, &result);
    if (!execute_success) return result;

    return result;
}

static bool cpu_fetch_instruction(cpu_t *cpu, cpu_state_t *result) {
    uint8_t bytes[2];
    bool    success = memory_read_bytes(cpu->memory, cpu->PC, bytes, sizeof(bytes));
    if (!success) {
        result->status = CPU_FETCH_FAILED;
        return false;
    }

    result->opcode = (bytes[0] << 8) | bytes[1];
    cpu->PC += 2;
    return true;
}

static bool cpu_execute_instruction(cpu_t *cpu, cpu_state_t *result) {
    switch (result->opcode & N1) {
        case 0x0000:
            switch (result->opcode) {
                case 0x00E0:
                    *cpu->display = display_create();
                    return true;
                default:
                    // TODO: Change to CPU_INSTRUCTION_INVALID
                    result->status = CPU_INSTRUCTION_NOT_IMPLEMENTED;
                    return false;
            }
        case 0x1000:
            cpu->PC = result->opcode & MA;
            return true;
        case 0x6000:
            cpu->V[EXTRACT_N2(result->opcode)] = result->opcode & B2;
            return true;
        case 0x7000:
            cpu->V[EXTRACT_N2(result->opcode)] += result->opcode & B2;
            return true;
        case 0xA000:
            cpu->I = result->opcode & MA;
            return true;
        case 0xD000: {
            uint8_t x   = cpu->V[EXTRACT_N2(result->opcode)];
            uint8_t y   = cpu->V[EXTRACT_N3(result->opcode)];
            uint8_t h   = EXTRACT_N4(result->opcode);
            cpu->V[0xF] = display_draw_sprite(cpu->display, x, y, h, &cpu->memory->data[cpu->I]);
            return true;
        }
        default:
            // TODO: Change to CPU_INSTRUCTION_INVALID
            result->status = CPU_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }

    return true;
}
