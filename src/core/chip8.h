#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "font.h"

#define MEMORY_SIZE       (4 * 1024) // 4KB; per specification
#define STACK_SIZE        16         // Arbitrary value
#define ADDRESS_SIZE      0xFFF      // 12-bits; per specification
#define PROGRAM_START     0x200      // Per specification
#define DISPLAY_WIDTH     64         // Per specification; scaled by driver
#define DISPLAY_HEIGHT    32         // Per specification; scaled by driver
#define FRAMES_PER_SECOND 60         // Per specification

typedef enum {
    CHIP8_OK = 0,
    CHIP8_FETCH_FAILED,
    CHIP8_INSTRUCTION_INVALID,
    CHIP8_INSTRUCTION_NOT_IMPLEMENTED,
    CHIP8_STACK_EMPTY,
    CHIP8_STACK_FULL,
} chip8_status_t;

typedef struct {
    chip8_status_t status;             // Latest emulator status
    uint16_t       opcode;             // Last processed opcode
    bool           frame_buffer_dirty; // If the display changed and must redraw
    bool           sound_timer_set;    // If the sound timer was enabled
} chip8_state_t;

typedef struct {
    // Core emulator state
    uint8_t  memory[MEMORY_SIZE];                     // Available memory
    uint16_t pc;                                      // Current memory address
    uint16_t i;                                       // Arbitrary address within memory
    uint8_t  v[16];                                   // Arbitrary variable registers
    uint16_t stack[16];                               // Subroutine return addresses
    int8_t   stack_pointer;                           // Current position within stack
    uint8_t  delay_timer;                             // Value of delay timer
    uint8_t  sound_timer;                             // Value of sound timer
    bool     display[DISPLAY_WIDTH * DISPLAY_HEIGHT]; // Active frame buffer
    // Meta-state for debugging and configuration
    font_type_t font;          // Active font
    bool        playing_sound; // If sound is currently being played
} chip8_t;

/**
 * Initializes the CHIP-8.
 *
 * In addition to allocating memory for the emulator, this function also
 * ensures that the emulator is correctly reset to its default state,
 * loads the configured `DEFAULT_FONT` into memory, and stores the provided
 * random number generator callback.
 *
 * @param chip8 - The CHIP-8 to initialize
 * @param generator - A callback that generates a random number
 */
void chip8_init(chip8_t *chip8, uint8_t (*generator)(void));

/**
 * Loads the requested font into memory.
 *
 * @param chip8 - The CHIP-8 to load the font into
 * @param type - The font type to load
 * @returns If the font was loaded successfully
 */
bool chip8_load_font(chip8_t *chip8, font_type_t type);

/**
 * Loads a program into memory and resets the CHIP-8 to its initial state.
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
 * An instruction cycle consists of a single internal fetch, decode, and
 * execute loop. This loop should therefore be called `INSTRUCTIONS_PER_SECOND`
 * times per second.
 *
 * Due to the various points of failure, the return value must be inspected
 * to determine what happened during the cycle and if the emulator can remain
 * active.
 *
 * @param chip8 - The CHIP-8 to run
 * @returns The emulator state after running the cycle
 */
chip8_state_t chip8_run_cycle(chip8_t *chip8);

/**
 * Fetches the next instruction from RAM.
 *
 * Since the CHIP-8's memory consists of 8-bit values, but instructions consist
 * of 16-bit values, this function is responsible for reading and combining two
 * consecutive bytes of program code into a single opcode.
 *
 * The provided result variable must default to a successful state, within
 * which the parsed instruction will be written on success. The return value
 * indicates if the fetch succeeded, which is to be used as a signal for
 * terminating the CPU cycle early.
 *
 * @param chip8 - The CHIP-8 to read the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was read successfully
 */
static bool chip8_fetch_instruction(chip8_t *cpu, chip8_state_t *result);

/**
 * Decodes and executes a single opcode.
 *
 * The provided result variable must default to a successful state with a valid
 * opcode, which will be read to execute the instruction. The return value
 * indicates if the execution succeeded, which is to be used as a signal for
 * terminating the CPU cycle early.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_instruction(chip8_t *chip8, chip8_state_t *result);

/**
 * Processes system (0x0xxx) instructions.
 *
 * Behaves in the same way as `chip8_execute_instruction`, except it only
 * handles the instructions whose first digit is 0.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_system_instruction(chip8_t *chip8, chip8_state_t *result);

/**
 * Processes arithmetic (0x8xxx) instructions.
 *
 * Behaves in the same way as `chip8_execute_instruction`, except it only
 * handles the instructions whose first digit is 8.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_arithmetic_instruction(chip8_t *chip8, chip8_state_t *result);

/**
 * Processes drawing (0xDxxx) instructions.
 *
 * Behaves in the same way as `chip8_execute_instruction`, except it only
 * handles the instructions whose first digit is D.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_draw_instruction(chip8_t *chip8, chip8_state_t *result);

/**
 * Processes keypress (0xExxx) instructions.
 *
 * Behaves in the same way as `chip8_execute_instruction`, except it only
 * handles the instructions whose first digit is E.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_keypress_instruction(chip8_t *chip8, chip8_state_t *result);

/**
 * Processes miscellaneous (0xFxxx) instructions.
 *
 * Behaves in the same way as `chip8_execute_instruction`, except it only
 * handles the instructions whose first digit is F.
 *
 * @param chip8 - The CHIP-8 to execute the instruction
 * @param result - The end result of running the entire instruction cycle
 * @returns If the opcode was successfully executed
 */
static bool chip8_execute_misc_instruction(chip8_t *chip8, chip8_state_t *result);
