#include "memory.h"
#include "unity_fixture.h"

TEST_GROUP(Memory);

static memory_t memory;

TEST_SETUP(Memory) {
    memory = memory_create();
}

TEST_TEAR_DOWN(Memory) {}

TEST(Memory, WriteAndReadSingleValue) {
    uint8_t  value   = 0xAB;
    uint16_t address = 0x200;

    bool write_success = memory_write(&memory, address, value);
    TEST_ASSERT_TRUE(write_success);
    TEST_ASSERT_EQUAL_UINT8(value, memory.data[address]);

    uint8_t read_value;
    bool    read_success = memory_read(&memory, address, &read_value);
    TEST_ASSERT_TRUE(read_success);
    TEST_ASSERT_EQUAL_UINT8(value, read_value);
}

TEST(Memory, WriteAndReadInvalidSingleValue) {
    uint8_t  value   = 0xAB;
    uint16_t address = MEMORY_SIZE + 1;

    bool write_success = memory_write(&memory, address, value);
    TEST_ASSERT_FALSE(write_success);
    TEST_ASSERT_EQUAL_UINT8(0, memory.data[address]);

    uint8_t read_value;
    bool    read_success = memory_read(&memory, address, &read_value);
    TEST_ASSERT_FALSE(read_success);
    TEST_ASSERT_EQUAL_UINT8(0, read_value);
}

TEST(Memory, WriteAndReadMultipleValues) {
    uint8_t  values[5] = {0xAB, 0xAC, 0xAD, 0xAE, 0xAF};
    uint16_t address   = 0x200;

    bool write_success = memory_write_bytes(&memory, address, values, sizeof(values));
    TEST_ASSERT_TRUE(write_success);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(values, &memory.data[address], sizeof(values));

    uint8_t read_values[5];
    bool    read_success = memory_read_bytes(&memory, address, read_values, sizeof(read_values));
    TEST_ASSERT_TRUE(read_success);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(values, read_values, sizeof(read_values));
}

TEST(Memory, WriteAndReadInvalidMultipleValues) {
    uint8_t  empty[5]  = {0};
    uint8_t  values[5] = {0xAB, 0xAC, 0xAD, 0xAE, 0xAF};
    uint16_t address   = MEMORY_SIZE + 1;

    bool write_success = memory_write_bytes(&memory, address, values, sizeof(values));
    TEST_ASSERT_FALSE(write_success);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(empty, &memory.data[address], sizeof(values));

    uint8_t read_values[5];
    bool    read_success = memory_read_bytes(&memory, address, read_values, sizeof(read_values));
    TEST_ASSERT_FALSE(read_success);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(empty, read_values, sizeof(read_values));
}
