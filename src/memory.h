#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_SIZE  (4 * 1024) // 4KB RAM per specification
#define ADDRESS_SIZE 0xFFF      // 12-bit addresses per specification

typedef struct {
    uint8_t data[MEMORY_SIZE];
} memory_t;

/**
 * Initializes the memory module.
 *
 * Allocates zeroed out `MEMORY_SIZE` bytes of memory;
 *
 * Due to the very small and fixed memory size required by the CHIP-8, memory
 * can be allocated on the stack, so there is no corresponding cleanup function.
 */
memory_t memory_create(void);

/**
 * Reads a single value from memory.
 *
 * Validates that the requested address is within bounds.
 *
 * @param memory - The memory module to read from
 * @param address - The address to read from
 * @param value - The value read
 * @returns If reading was successful
 */
bool memory_read(const memory_t *memory, uint16_t address, uint8_t *value);

/**
 * Reads several values from memory.
 *
 * Validates that the requested address range is within bounds.
 *
 * @param memory - The memory module to read from
 * @param address - The address to start reading from
 * @param size - The amount of values to read
 * @param buffer - The values read
 * @returns If reading was successful
 */
bool memory_read_bytes(
    const memory_t *memory,
    uint16_t        address,
    uint8_t        *buffer,
    size_t          size
);

/**
 * Writes a single value to memory
 *
 * Validates that the requested address is within bounds.
 *
 * @param memory - The memory module to write to
 * @param address - The address to write to
 * @param value - The value to write
 * @returns If writing was successful
 */
bool memory_write(memory_t *memory, uint16_t address, uint8_t value);

/**
 * Writes several values to memory.
 *
 * Validates that the requested address range is within bounds.
 *
 * @param memory - The memory module to writo te
 * @param address - The address to start writing from
 * @param buffer - The values to write
 * @param size - The amount of values to write
 * @returns If writing was successful
 */
bool memory_write_bytes(
    memory_t      *memory,
    uint16_t       address,
    const uint8_t *buffer,
    size_t         size
);
