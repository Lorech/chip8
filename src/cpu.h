#pragma once

#include "display.h"
#include "font.h"
#include "memory.h"
#include "stack.h"
#include "timer.h"

#define PROGRAM_START 0x200 // Legacy system specification
#ifndef INSTRUCTIONS_PER_SECOND
#define INSTRUCTIONS_PER_SECOND 700
#endif

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

/**
 * Runs a single CPU cycle.
 *
 * A CPU cycle consists of a single internal fetch, decode, execute loop. This
 * loop should therefore be called `INSTRUCTIONS_PER_SECOND` times per second.
 *
 * The return value indicates a success state as far as recovering from the
 * cycle is concerned, i.e., if the returned value is false, that should be
 * treated as a signal that something fatally wrong ocurred during the cycle,
 * and the emulator should terminate. Unimplemented or non-critical issues will
 * therefore still return success, merely logging the issue if logging is on.
 *
 * @param cpu - The CPU module to run the cycle
 * @returns If the cycle executed successfully or not
 */
bool cpu_run_cycle(cpu_t *cpu);

/**
 * Fetches the next instruction from the ROM.
 *
 * Since the CHIP-8's memory consists of 8-bit values, but instructions consist
 * of 16-bit values, this function is responsible for reading and combining two
 * consecutive bytes of program code into a single opcode.
 *
 * If the resulting opcode is valid, true will be returned.
 *
 * @param cpu - The CPU module to read the instruction
 * @param opcode - The resulting opcode, made up of two bytes of code
 * @returns If the opcode could be successfully read
 */
static bool cpu_fetch_instruction(cpu_t *cpu, uint16_t *opcode);

/**
 * Decodes and executes a single opcode.
 *
 * Takes in an opcode, decodes it, and executes its instruction. The return
 * value indicates a fatal, irrecoverable error, not if the opcode wasn't
 * handled at all. In these scenarios, a log will be emitted if logging is
 * enabled, as such an error is a program issue, not a CPU issue.
 *
 * @param cpu - The CPU module to execute the instruction
 * @param opcode - The instruction to execute
 * @returns If the opcode could be safely handled
 */
static bool cpu_execute_instruction(cpu_t *cpu, uint16_t opcode);
