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
