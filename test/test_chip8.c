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
    uint8_t    *old_memory = &chip8.memory[FONT_START];

    uint8_t zero[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    bool    success = chip8_load_font(&chip8, FONT_DREAM6800);
    TEST_ASSERT_TRUE_MESSAGE(success, "Loading valid font should not fail.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(zero, &chip8.memory[FONT_START], sizeof(zero), "Loaded font should update memory.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FONT_DREAM6800, chip8.font, "Loaded font should be set as active font");
}

TEST(CHIP8, LoadInvalidFont) {
    // chip8_init loads the default font, which can be configured when
    // compiling, so we need to pull the starting values to compare against.
    font_type_t old_font   = chip8.font;
    uint8_t    *old_memory = &chip8.memory[FONT_START];

    bool success = chip8_load_font(&chip8, FONT_COUNT);
    TEST_ASSERT_FALSE_MESSAGE(success, "Loading invalid font should fail.");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(old_memory, &chip8.memory[FONT_START], 5, "Ignored font should not update memory.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(old_font, chip8.font, "Ignored font should not update active font");
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
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(program, &chip8.memory[PROGRAM_START], sizeof(program), "Memory should contain the new program if loading succeeded.");
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
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(empty_program, &chip8.memory[PROGRAM_START], sizeof(empty_program), "Memory should not change if loading failed.");
}

TEST(CHIP8, ClearScreen) {
    uint8_t       program[2] = {0x00, 0xE0};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x00E0, result.opcode, "Should create \"Clear Screen\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
}

TEST(CHIP8, ExecuteSubroutine) {
    uint8_t program[6] = {0x22, 0x04, 0x00, 0xE0, 0x00, 0xEE};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8_state_t call_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x2204, call_result.opcode, "Should create \"Execute Subroutine\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, call_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x204, chip8.pc, "Should jump PC to provided address.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, chip8.stack_pointer, "Should increment stack pointer.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x202, chip8.stack[0], "Should push PC to stack.");

    chip8_state_t return_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x00EE, return_result.opcode, "Should create \"Return from Subroutine\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, return_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x202, chip8.pc, "Should jump PC to stored address.");
    TEST_ASSERT_EQUAL_INT8_MESSAGE(-1, chip8.stack_pointer, "Should decrement stack pointer.");
}

TEST(CHIP8, Jump) {
    uint8_t       program[2] = {0x11, 0x23};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x1123, result.opcode, "Should create \"Jump\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x123, chip8.pc, "Should jump PC to provided address.");
}

TEST(CHIP8, SetVariable) {
    uint8_t       program[2] = {0x61, 0x23};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x6123, result.opcode, "Should create \"Set Variable\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x23, chip8.v[1], "Should update value of V1.");
}

TEST(CHIP8, AddToVariable) {
    uint8_t program[4] = {0x61, 0x20, 0x71, 0x40};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    // Set an initial value in V1
    chip8_state_t result_set = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x6120, result_set.opcode, "Should create \"Set Variable\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result_set.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x20, chip8.v[1], "Should update value of V1.");

    // Add to the value in V1
    chip8_state_t result_add = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x7140, result_add.opcode, "Should create \"Add to Variable\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result_add.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 4, chip8.pc, "Should advance PC.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x60, chip8.v[1], "Should add to the existing value of V1.");
}

TEST(CHIP8, SkipStaticEqual) {
    uint8_t program[6] = {0x30, 0x01, 0x30, 0x02};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8.v[0] = 0x02;

    chip8_state_t first_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x3001, first_result.opcode, "Should create \"Skip if Variable Equals\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, first_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should not skip, should advance PC.");

    chip8_state_t second_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x3002, second_result.opcode, "Should create \"Skip if Variable Equals\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, second_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 6, chip8.pc, "Should skip, should advance PC twice.");
}

TEST(CHIP8, SkipStaticNotEqual) {
    uint8_t program[6] = {0x40, 0x02, 0x40, 0x01};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8.v[0] = 0x02;

    chip8_state_t first_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x4002, first_result.opcode, "Should create \"Skip if Variable Not Equals\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, first_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should not skip, should advance PC.");

    chip8_state_t second_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x4001, second_result.opcode, "Should create \"Skip if Variable Not Equals\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, second_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 6, chip8.pc, "Should skip, should advance PC twice.");
}

TEST(CHIP8, SkipVariablesEqual) {
    uint8_t program[6] = {0x50, 0x10, 0x50, 0x00};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8.v[0] = 0x01;
    chip8.v[1] = 0x02;

    chip8_state_t first_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x5010, first_result.opcode, "Should create \"Skip if Variables Equal\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, first_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should not skip, should advance PC.");

    chip8_state_t second_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x5000, second_result.opcode, "Should create \"Skip if Variables Equal\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, second_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 6, chip8.pc, "Should skip, should advance PC twice.");
}

TEST(CHIP8, SkipVariablesNotEqual) {
    uint8_t program[6] = {0x90, 0x00, 0x90, 0x10};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8.v[0] = 0x01;
    chip8.v[1] = 0x02;

    chip8_state_t first_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x9000, first_result.opcode, "Should create \"Skip if Variables Not Equal\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, first_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should not skip, should advance PC.");

    chip8_state_t second_result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x9010, second_result.opcode, "Should create \"Skip if Variables Not Equal\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, second_result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 6, chip8.pc, "Should skip, should advance PC twice.");
}

TEST(CHIP8, SetIndex) {
    uint8_t       program[2] = {0xA1, 0x23};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0xA123, result.opcode, "Should create \"Set Index\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x123, chip8.i, "Should update value of I.");
}

TEST(CHIP8, DrawSprite) {
    uint8_t program[2] = {0xD0, 0x15};
    bool    loaded     = chip8_load_program(&chip8, program, sizeof(program));

    chip8.i             = 0x300;
    chip8.v[0]          = 1;
    chip8.v[1]          = 2;
    chip8.memory[0x300] = 0xF0;
    chip8.memory[0x301] = 0x90;
    chip8.memory[0x302] = 0x90;
    chip8.memory[0x303] = 0x90;
    chip8.memory[0x304] = 0xF0;

    chip8_state_t result = chip8_run_cycle(&chip8);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0xD015, result.opcode, "Should create \"Draw Sprite\".");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_OK, result.status, "Should be implemented.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.pc, "Should advance PC.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, chip8.v[0xF], "Should not set VF.");
    TEST_ASSERT_TRUE_MESSAGE(chip8.display[(2 * DISPLAY_WIDTH) + 1], "Top-left pixel of sprite should be ON.");
    TEST_ASSERT_TRUE_MESSAGE(chip8.display[(6 * DISPLAY_WIDTH) + 4], "Bottom-right pixel of sprite should be ON.");
}
