#pragma once

#include <stdint.h>

#define MEMORY_SIZE  (4 * 1024) // 4KB RAM per specification
#define MEMORY_START 0x200      // General convention
#define ADDRESS_SIZE 0xFFF      // 12-bit addresses per specification

typedef struct {
    uint8_t data[MEMORY_SIZE];
} memory_t;

/**
 * Initializes the memory module.
 *
 * Due to the very small and fixed memory size required by the CHIP-8, memory
 * can be allocated on the stack, so there is no corresponding cleanup function.
 */
memory_t memory_create(void);

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
