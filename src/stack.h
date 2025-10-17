#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t   size;
    uint8_t   top;
    uint16_t *addresses;
} chipstack_t;

/**
 * Initializes a new stack.
 *
 * @param size - The maximum amount of items the stack can hold
 * @returns The created stack
 */
chipstack_t stack_create(uint8_t size);

/**
 * Destroys an existing stack.
 *
 * @param stack - The stack to free from memory
 */
void stack_destroy(chipstack_t *stack);

/**
 * Pushes a new address onto the stack.
 *
 * As the CHIP-8 only uses the stack to store addresses, the function validates
 * that the provided item is no longer than 12 bits (the maximum address length).
 *
 * @param stack - The stack to be modified
 * @param address - The address to push
 * @returns If the push was successful or not
 */
bool stack_push(chipstack_t *stack, uint16_t address);

/**
 * Pops an address from the stack.
 *
 * @param stack - The stack to be modified
 * @param address - The address that was popped, or NULL if unsuccessful
 * @returns If the pop was successful or not
 */
bool stack_pop(chipstack_t *stack, uint16_t *address);
