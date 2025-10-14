#include "memory.h"

#include <stdio.h>

memory_t memory_create(void) {
    memory_t memory = {0};
    return memory;
}

uint8_t *memory_read(memory_t *memory, uint16_t address) {
    if (address > ADDRESS_SIZE) return NULL;
    return &memory->data[address];
}

uint8_t *memory_write(memory_t *memory, uint16_t address, uint8_t value) {
    if (address > ADDRESS_SIZE) return NULL;
    memory->data[address] = value;
    return &memory->data[address];
}
