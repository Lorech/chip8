#include "chip8.h"
#include "font.h"
#include "unity_fixture.h"

TEST_GROUP(CHIP8);

static chip8_t chip8;

TEST_SETUP(CHIP8) {
    chip8_init(&chip8);
}

TEST_TEAR_DOWN(CHIP8) {}

TEST(CHIP8, LoadValidFont) {
    // chip8_init loads the default font, which can be configured when
    // compiling, so we need to pull the starting values to compare against.
    font_type_t old_font   = chip8.font;
    uint8_t    *old_memory = &chip8.ram[FONT_START];

    uint8_t zero[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    bool    success = chip8_load_font(&chip8, FONT_DREAM6800);
    TEST_ASSERT_TRUE_MESSAGE(success, "Loading valid font should not fail.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(zero, &chip8.ram[FONT_START], sizeof(zero), "Loaded font should update memory.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FONT_DREAM6800, chip8.font, "Loaded font should be set as active font");
}

TEST(CHIP8, LoadInvalidFont) {
    // chip8_init loads the default font, which can be configured when
    // compiling, so we need to pull the starting values to compare against.
    font_type_t old_font   = chip8.font;
    uint8_t    *old_memory = &chip8.ram[FONT_START];

    bool success = chip8_load_font(&chip8, FONT_COUNT);
    TEST_ASSERT_FALSE_MESSAGE(success, "Loading invalid font should fail.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(old_memory, &chip8.ram[FONT_START], 5, "Ignored font should not update memory.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(old_font, chip8.font, "Ignored font should nont update active font");
}

TEST(CHIP8, LoadValidProgram) {
    uint8_t program[MEMORY_SIZE - PROGRAM_START] = {0x00, 0xE0};
    uint8_t empty_vars[16]                       = {0};
    // Change state so we can validate reset procedure.
    chip8.pc          = 0x204;
    chip8.i           = 0x500;
    chip8.delay_timer = 0x20;
    chip8.sound_timer = 0x20;
    chip8.v[1]        = 0x40;

    bool success = chip8_load_program(&chip8, program, sizeof(program));
    TEST_ASSERT_TRUE_MESSAGE(success, "Loading valid program should not fail.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PROGRAM_START, chip8.pc, "PC should reset if loading succeeded.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, chip8.i, "I should reset if loading succeeded.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, chip8.delay_timer, "Delay timer should reset if loading succeeded.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, chip8.sound_timer, "Sound timer should reset if loading succeeded.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(empty_vars, &chip8.v, sizeof(empty_vars), "Variables should reset if loading succeeded.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(program, &chip8.ram[PROGRAM_START], sizeof(program), "Memory should contain the new program if loading succeeded.");
}

TEST(CHIP8, LoadInvalidProgram) {
    uint8_t empty_program[MEMORY_SIZE - PROGRAM_START] = {0};
    uint8_t expected_vars[16]                          = {0x00, 0x40};
    // Change state so we can validate reset procedure.
    chip8.pc          = 0x204;
    chip8.i           = 0x500;
    chip8.delay_timer = 0x20;
    chip8.sound_timer = 0x20;
    chip8.v[1]        = 0x40;

    // Intentionally create a program that spans entire memory range to test overflow handling.
    // Switch some bytes of the program to test memory mutation not happening on overflow.
    uint8_t program[MEMORY_SIZE] = {0};
    program[0]                   = 0xE0;
    program[256]                 = 0xA0;
    program[512]                 = 0xA0;
    program[768]                 = 0xA0;
    program[1032]                = 0xE0;

    bool success = chip8_load_program(&chip8, program, sizeof(program));
    TEST_ASSERT_FALSE_MESSAGE(success, "Loading invalid program should fail.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x204, chip8.pc, "PC should not reset if loading failed.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x500, chip8.i, "I should not reset if loading failed.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x20, chip8.delay_timer, "Delay timer should not reset if loading failed.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x20, chip8.sound_timer, "Sound timer should not reset if loading failed.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_vars, &chip8.v, sizeof(expected_vars), "Variables should not reset if loading failed.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(empty_program, &chip8.ram[PROGRAM_START], sizeof(empty_program), "Memory should not change if loading failed.");
}

TEST(CHIP8, RunCycle) {
    uint8_t       program[2] = {0x00, 0xE0};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x00E0, result.opcode, "Should create \"Clear Screen\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
}
