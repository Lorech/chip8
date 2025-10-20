#include "chip8.h"
#include "font.h"
#include "memory.h"
#include "stack.h"
#include "timer.h"
#include "unity_fixture.h"

TEST_GROUP(CHIP8);

static chip8_t     chip8;
static memory_t    memory;
static chipstack_t stack;
static display_t   display;
static timer_t     timer;

TEST_SETUP(CHIP8) {
    memory  = memory_create();
    stack   = stack_create(4);
    display = display_create();

    timer_init(&timer);
    chip8_init(&chip8, &memory, &stack, &timer, &display);
}

TEST_TEAR_DOWN(CHIP8) {
    stack_destroy(&stack);
}

TEST(CHIP8, LoadFont) {
    bool dream_success = chip8_load_font(&chip8, FONT_DREAM6800);
    TEST_ASSERT_TRUE(dream_success);
    uint8_t dream_zero[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dream_zero, &chip8.memory->data[FONT_START], sizeof(dream_zero));
    TEST_ASSERT_EQUAL_UINT8(FONT_DREAM6800, chip8.font);

    bool cosmac_success = chip8_load_font(&chip8, FONT_COSMACVIP);
    TEST_ASSERT_TRUE(cosmac_success);
    uint8_t cosmac_zero[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(cosmac_zero, &chip8.memory->data[FONT_START], sizeof(cosmac_zero));
    TEST_ASSERT_EQUAL_UINT8(FONT_COSMACVIP, chip8.font);
}

TEST(CHIP8, LoadValidProgram) {
    // NOTE: Not a valid CHIP-8 program; copied from font testing.
    uint8_t  program[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    uint16_t size       = 5;
    chip8.PC            = 0x204; // Allow for validating reset procedure.

    bool success = chip8_load_program(&chip8, program, size);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_UINT8(PROGRAM_START, chip8.PC);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(program, &chip8.memory->data[chip8.PC], size);
}

TEST(CHIP8, LoadInvalidProgram) {
    uint8_t empty[MEMORY_SIZE] = {0};
    chip8.PC                   = 0x204; // Allow for validating reset procedure.

    // Intentionally create a program that spans entire memory range to test overflow handling.
    // Switch some bytes of the program to test memory mutation not happening on overflow.
    uint8_t program[MEMORY_SIZE] = {0};
    program[0]                   = 0xE0;
    program[256]                 = 0xA0;
    program[512]                 = 0xA0;
    program[768]                 = 0xA0;
    program[1032]                = 0xE0;

    bool success = chip8_load_program(&chip8, program, sizeof(program));
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_UINT8(0x204, chip8.PC);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(empty, &chip8.memory->data[chip8.PC], sizeof(MEMORY_SIZE - chip8.PC));
}

TEST(CHIP8, RunCycle) {
    uint8_t       program[2] = {0xAA, 0xBB};
    bool          loaded     = chip8_load_program(&chip8, program, sizeof(program));
    chip8_state_t result     = chip8_run_cycle(&chip8);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CHIP8_INSTRUCTION_NOT_IMPLEMENTED, result.status, "All chip8 instructions should be unimplemented");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0xAABB, result.opcode, "Fetched opcode should join two bytes together");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(PROGRAM_START + 2, chip8.PC, "Valid fetch should advance the program counter by 2");
}
