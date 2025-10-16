#pragma once

#include <stdint.h>

#include "font.h"

#define MEMORY_SIZE   (4 * 1024) // 4KB RAM per specification
#define ADDRESS_SIZE  0xFFF      // 12-bit addresses per specification
#define PROGRAM_START 0x200      // Legacy system specification

typedef struct {
    uint8_t     data[MEMORY_SIZE];
    font_type_t font;
} memory_t;

/**
 * Initializes the memory module.
 *
 * The initialization step:
 *  1. Allocates zeroed out `MEMORY_SIZE` bytes of memory;
 *  2. Loads `DEFAULT_FONT` into memory starting from `FONT_START`.
 *
 * Due to the very small and fixed memory size required by the CHIP-8, memory
 * can be allocated on the stack, so there is no corresponding cleanup function.
 */
memory_t memory_create(void);

/**
 * Loads the provided program into memory.
 *
 * Validates that the program will fit into memory, returning NULL if not.
 *
 * @param memory - The memory module to load the font to
 * @param program - The program to load into memory
 * @param size - The size of the program
 * @returns Pointer to the first address of the program, or NULL if invalid
 */
uint8_t *memory_load_program(memory_t *memory, uint8_t *program, uint16_t size);

/**
 * Loads the requested font into memory.
 *
 * Validates the requested font, returning NULL if not. The font will be loaded
 * into memory starting from memory address `FONT_START`, which is validated to
 * fit within the allocated memory, returning NULL if not.
 *
 * @param memory - The memory module to load the font to
 * @param type - The font type to load into memory
 * @returns Pointer to the first address of the font, or NULL for invalid font
 */
uint8_t *memory_load_font(memory_t *memory, font_type_t type);

/**
 * Grants access to the memory at the requested address.
 *
 * As per CHIP-8 specification, the requested address must fit within 12 bits.
 * If it does not, NULL will be returned.
 *
 * @param memory - The memory module to read from
 * @param address - The starting address to read from
 * @returns Pointer to the requested address, or NULL if the address overflows.
 */
uint8_t *memory_read(memory_t *memory, uint16_t address);

/**
 * Writes a value to memory at the requested address.
 *
 * As per CHIP-8 specification, the requested address must fit within 12 bits.
 * If it does not, NULL will be returned.
 *
 * @param memory - The memory module to write to
 * @param address - The starting address to write to
 * @param value - The value to write to the address
 * @returns Pointer to the written address, or NULL if the address overflows.
 */
uint8_t *memory_write(memory_t *memory, uint16_t address, uint8_t value);
