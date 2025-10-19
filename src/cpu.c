#include "cpu.h"

#include <stdio.h>
#include <string.h>

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
    switch (result->opcode) {
        default:
            // TODO: Change to CPU_INSTRUCTION_INVALID once all instructions are defined.
            result->status = CPU_INSTRUCTION_NOT_IMPLEMENTED;
            return false;
    }

    return true;
}
