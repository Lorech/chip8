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

// Bitmasks
#define N1 0xF000 // First nibble - instruction group
#define N2 0x0F00 // Second nibble - register lookup
#define N3 0x00F0 // Third nibble - register lookup
#define N4 0x000F // Fourth nibble - 4-bit number
#define B1 0xFF00 // First byte - added for completion sake
#define B2 0x00FF // Second byte - 8-bit number
#define MA 0x0FFF // 12-bit memory address

typedef enum {
    CPU_OK = 0,
    CPU_FETCH_FAILED,
    CPU_INSTRUCTION_INVALID,
    CPU_INSTRUCTION_NOT_IMPLEMENTED,
} cpu_status_code_t;

typedef struct {
    cpu_status_code_t status;
    uint16_t          PC;
    uint16_t          opcode;
} cpu_state_t;

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
 * @param cpu - The CPU module to run the cycle
 * @returns The CPU state after running the cycle
 */
cpu_state_t cpu_run_cycle(cpu_t *cpu);

/**
 * Fetches the next instruction from the ROM.
 *
 * Since the CHIP-8's memory consists of 8-bit values, but instructions consist
 * of 16-bit values, this function is responsible for reading and combining two
 * consecutive bytes of program code into a single opcode.
 *
 * The result variable provided to the function must default to a successful
 * result, within which the parsed opcode will be written to on success. The
 * return value indicates if the fetch succeeded, which is to be used as a
 * signal for terminating the CPU cycle early.
 *
 * @param cpu - The CPU module to read the instruction
 * @param result - The end result of running the entire CPU cycle
 * @returns If the opcode could be successfully read
 */
static bool cpu_fetch_instruction(cpu_t *cpu, cpu_state_t *result);

/**
 * Decodes and executes a single opcode.
 *
 * The result variable provided to the function should default to a successful
 * result with a valid opcode, which will be read to execute the instruction.
 * The return value indicates if the instruction execution succeeded, which
 * is to be used as a signal for terminating the CPU cyucle early. If execution
 * failed, the rest of the result struct will be modified appropriately.
 *
 * @param cpu - The CPU module to execute the instruction
 * @param opcode - The instruction to execute
 * @returns If the opcode could be safely handled
 */
static bool cpu_execute_instruction(cpu_t *cpu, cpu_state_t *result);
