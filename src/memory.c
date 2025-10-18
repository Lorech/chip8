#include "memory.h"

#include <string.h>

memory_t memory_create(void) {
    memory_t memory = {0};
    return memory;
}

bool memory_read(const memory_t *memory, uint16_t address, uint8_t *value) {
    if (!value || address > ADDRESS_SIZE) return false;
    *value = memory->data[address];
    return true;
}

bool memory_read_bytes(
    const memory_t *memory,
    uint16_t        address,
    uint8_t        *buffer,
    size_t          size
) {
    if (!buffer || address + size > MEMORY_SIZE) return false;
    memcpy(buffer, &memory->data[address], size);
    return true;
}

bool memory_write(memory_t *memory, uint16_t address, uint8_t value) {
    if (address > ADDRESS_SIZE) return false;
    memory->data[address] = value;
    return true;
}

bool memory_write_bytes(
    memory_t      *memory,
    uint16_t       address,
    const uint8_t *buffer,
    size_t         size
) {
    if (!buffer || address + size > MEMORY_SIZE) return false;
    memcpy(&memory->data[address], buffer, size);
    return true;
}
