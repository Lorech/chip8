#pragma once

#include "display.h"
#include "memory.h"
#include "stack.h"
#include "timer.h"

typedef struct {
    uint16_t     PC;
    uint16_t     I;
    uint8_t      V[16];
    memory_t    *memory;
    chipstack_t *stack;
    timer_t     *timer;
    display_t   *display;
} cpu_t;

/**
 * Initializes the CPU module.
 *
 * The CPU module in this interpretation is essentially the entire emulator -
 * it is the orchestrator that manages every subsystem. This initializer will
 * therefore assign all the subsystems, as well as zero out it's own internal
 * variables and registers.
 *
 * @param cpu - The CPU to initialize
 * @param memory - The memory module to assign to the CPU
 * @param stack - The stack module to assign to the CPU
 * @param timer - The timer module to assign to the CPU
 * @param display - The display module to assign to the CPU
 */
void cpu_init(
    cpu_t       *cpu,
    memory_t    *memory,
    chipstack_t *stack,
    timer_t     *timer,
    display_t   *display
);
