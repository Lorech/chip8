#include "font.h"
#include "memory.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(Memory);

static memory_t memory;

TEST_SETUP(Memory) {
    memory = memory_create();
}

TEST_TEAR_DOWN(Memory) {}

TEST(Memory, WriteAndRead) {
    uint8_t  value   = 0xAB;
    uint16_t address = 0x200;

    uint8_t *write_ptr = memory_write(&memory, address, value);
    TEST_ASSERT_NOT_NULL(write_ptr);
    TEST_ASSERT_EQUAL_UINT8(value, *write_ptr);

    uint8_t *read_ptr = memory_read(&memory, address);
    TEST_ASSERT_NOT_NULL(read_ptr);
    TEST_ASSERT_EQUAL_UINT8(value, *read_ptr);
}

TEST(Memory, NullOnInvalidAddress) {
    TEST_ASSERT_NULL(memory_read(&memory, 0xFFF + 1));
    TEST_ASSERT_NULL(memory_write(&memory, 0xFFF + 1, 0xAB));
}

TEST(Memory, LoadFont) {
    uint8_t *dream_address = memory_load_font(&memory, FONT_DREAM6800);
    TEST_ASSERT_NOT_NULL(dream_address);
    uint8_t dream_zero[5]     = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    uint8_t dream_zero_length = sizeof(dream_zero);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dream_zero, dream_address, dream_zero_length);
    TEST_ASSERT_EQUAL_UINT8(FONT_DREAM6800, memory.font);

    uint8_t *cosmac_address = memory_load_font(&memory, FONT_COSMACVIP);
    TEST_ASSERT_NOT_NULL(cosmac_address);
    uint8_t cosmac_zero[5]     = {0xF0, 0x90, 0x90, 0x90, 0xF0};
    uint8_t cosmac_zero_length = sizeof(cosmac_zero);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(cosmac_zero, cosmac_address, cosmac_zero_length);
    TEST_ASSERT_EQUAL_UINT8(FONT_COSMACVIP, memory.font);
}

TEST(Memory, LoadProgram) {
    // NOTE: Not a valid CHIP-8 program; copied from font testing.
    uint8_t program[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    uint8_t size       = 5;

    uint8_t *address = memory_load_program(&memory, program, size);
    TEST_ASSERT_NOT_NULL(address);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(program, address, size);
}
