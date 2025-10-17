#include "cpu.h"

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
