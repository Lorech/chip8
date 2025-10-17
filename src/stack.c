#include "stack.h"

#include <stdlib.h>

#include "memory.h"

chipstack_t stack_create(uint8_t size) {
    chipstack_t stack;
    stack.size      = size;
    stack.top       = 0;
    stack.addresses = malloc(size * sizeof(uint16_t));
    return stack;
};

void stack_destroy(chipstack_t *stack) {
    free(stack->addresses);
    stack->addresses = NULL;
};

bool stack_push(chipstack_t *stack, uint16_t address) {
    if (stack->top >= stack->size) return false;
    if (address > ADDRESS_SIZE) return false;
    stack->addresses[stack->top++] = address;
    return true;
};

bool stack_pop(chipstack_t *stack, uint16_t *address) {
    if (stack->top == 0) return false;
    *address = stack->addresses[--stack->top];
    return true;
};
