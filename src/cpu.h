#pragma once

#include "display.h"
#include "font.h"
#include "memory.h"
#include "stack.h"
#include "timer.h"

#define PROGRAM_START 0x200 // Legacy system specification

typedef struct {
    uint16_t     PC;
    uint16_t     I;
    uint8_t      V[16];
    font_type_t  font;
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
 * therefore assign all the subsystems, zero out it's own internal
 * variables and registers, and load `DEFAULT_FONT` into memory.
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

/**
 * Resets an existing CPUs internal state to default.
 *
 * This function does not alter any subsystem state - only the parts the CPU
 * controls directly, i.e., the program counter, index register, and variable
 * registers.
 *
 * @param cpu - The CPU to reset
 */
void cpu_reset(cpu_t *cpu);

/**
 * Loads the requested font.
 *
 * Validates that the requested font exists and fits into memory, returning
 * false if not.
 *
 * @param cpu - The CPU module to load the font into
 * @param type - The font type to load
 * @returns If the font was loaded successfully
 */
bool cpu_load_font(cpu_t *cpu, font_type_t type);

/**
 * Loads a program into memory and resets the CPU to its initial state.
 *
 * Validates that the program will fit into memory, returning false if not.
 *
 * @param cpu - The CPU module to load the program into
 * @param program - The program to load
 * @param size - The size of the program
 * @returns If the program was successfully loaded or not
 */
bool cpu_load_program(cpu_t *cpu, const uint8_t *program, uint16_t size);
