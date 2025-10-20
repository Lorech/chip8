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

typedef enum {
    CHIP8_OK = 0,
    CHIP8_FETCH_FAILED,
    CHIP8_INSTRUCTION_INVALID,
    CHIP8_INSTRUCTION_NOT_IMPLEMENTED,
} chip8_status_t;

typedef struct {
    chip8_status_t status;
    uint16_t       PC;
    uint16_t       opcode;
} chip8_state_t;

typedef struct {
    uint16_t     PC;
    uint16_t     I;
    uint8_t      V[16];
    font_type_t  font;
    memory_t    *memory;
    chipstack_t *stack;
    timer_t     *timer;
    display_t   *display;
} chip8_t;

/**
 * Initializes the CHIP-8.
 *
 * The CHIP-8 is the orchestrator that manages every subsystem. This
 * initializer will assign all the subsystems, zero out it's own internal
 * variables and registers, and load `DEFAULT_FONT` into memory.
 *
 * @param chip8 - The CHIP-8 to initialize
 * @param memory - The memory module to assign to the CHIP-8
 * @param stack - The stack module to assign to the CHIP-8
 * @param timer - The timer module to assign to the CHIP-8
 * @param display - The display module to assign to the CHIP-8
 */
void chip8_init(
    chip8_t     *chip8,
    memory_t    *memory,
    chipstack_t *stack,
    timer_t     *timer,
    display_t   *display
);

/**
 * Resets an existing CHIP-8's internal state to default.
 *
 * This function does not alter any subsystem state - only the parts the
 * chip8 controls directly, i.e., the program counter, index register, and
 * variable registers.
 *
 * @param chip8 - The CHIP-8 to reset
 */
void chip8_reset(chip8_t *chip8);

/**
 * Loads the requested font.
 *
 * Validates that the requested font exists and fits into memory, returning
 * false if not.
 *
 * @param chip8 - The CHIP-8 to load the font into
 * @param type - The font type to load
 * @returns If the font was loaded successfully
 */
bool chip8_load_font(chip8_t *chip8, font_type_t type);

/**
 * Loads a program into memory and resets the CHIP-8 to its initial state.
 *
 * Validates that the program will fit into memory, returning false if not.
 *
 * @param chip8 - The CHIP-8 to load the program into
 * @param program - The program to load
 * @param size - The size of the program
 * @returns If the program was successfully loaded or not
 */
bool chip8_load_program(chip8_t *chip8, const uint8_t *program, uint16_t size);

/**
 * Runs a single instruction cycle.
 *
 * A single cycle consists of a single internal fetch, decode, execute loop.
 * This loop should therefore be called `INSTRUCTIONS_PER_SECOND` times per
 * second.
 *
 * @param chip8 - The CHIP-8 run the cycle
 * @returns The CPU state after running the cycle
 */
chip8_state_t chip8_run_cycle(chip8_t *chip8);

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
 * @param chip8 - The CHIP-8 to read the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode could be successfully read
 */
static bool chip8_fetch_instruction(chip8_t *cpu, chip8_state_t *result);

/**
 * Decodes and executes a single opcode.
 *
 * The result variable provided to the function should default to a successful
 * result with a valid opcode, which will be read to execute the instruction.
 * The return value indicates if the instruction execution succeeded, which
 * is to be used as a signal for terminating the CPU cycle early. If execution
 * failed, the rest of the result struct will be modified appropriately.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param opcode - The instruction to execute
 * @returns If the opcode could be safely handled
 */
static bool chip8_execute_instruction(chip8_t *chip8, chip8_state_t *result);
