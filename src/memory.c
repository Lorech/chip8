#include "memory.h"

#include <string.h>

#include "font.h"

memory_t memory_create(void) {
    memory_t memory = {0};
    memory_load_font(&memory, DEFAULT_FONT);
    return memory;
}

uint8_t *memory_load_font(memory_t *memory, font_type_t type) {
    font_data_t font = font_get(type);
    if (font.data == NULL) return NULL;
    if (font.size > MEMORY_SIZE - FONT_START) return NULL;
    memory->font = type;
    memcpy(&memory->data[FONT_START], font.data, font.size);
    return &memory->data[FONT_START];
}

uint8_t *memory_load_program(memory_t *memory, const uint8_t *program, uint16_t size) {
    if (size > MEMORY_SIZE - PROGRAM_START) return NULL;
    memcpy(&memory->data[PROGRAM_START], program, size);
    return &memory->data[PROGRAM_START];
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
