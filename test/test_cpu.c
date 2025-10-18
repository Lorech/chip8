#include "cpu.h"
#include "font.h"
#include "memory.h"
#include "stack.h"
#include "timer.h"
#include "unity_fixture.h"

TEST_GROUP(Cpu);

static cpu_t       cpu;
static memory_t    memory;
static chipstack_t stack;
static display_t   display;
static timer_t     timer;

TEST_SETUP(Cpu) {
    memory  = memory_create();
    stack   = stack_create(4);
    display = display_create();

    timer_init(&timer);
    cpu_init(&cpu, &memory, &stack, &timer, &display);
}

TEST_TEAR_DOWN(Cpu) {
    stack_destroy(&stack);
}

TEST(Cpu, LoadFont) {
    bool dream_success = cpu_load_font(&cpu, FONT_DREAM6800);
    TEST_ASSERT_TRUE(dream_success);
    uint8_t dream_zero[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dream_zero, &cpu.memory->data[FONT_START], sizeof(dream_zero));
    TEST_ASSERT_EQUAL_UINT8(FONT_DREAM6800, cpu.font);

    bool cosmac_success = cpu_load_font(&cpu, FONT_COSMACVIP);
    TEST_ASSERT_TRUE(cosmac_success);
    uint8_t cosmac_zero[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(cosmac_zero, &cpu.memory->data[FONT_START], sizeof(cosmac_zero));
    TEST_ASSERT_EQUAL_UINT8(FONT_COSMACVIP, cpu.font);
}

TEST(Cpu, LoadValidProgram) {
    // NOTE: Not a valid CHIP-8 program; copied from font testing.
    uint8_t  program[5] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
    uint16_t size       = 5;
    cpu.PC              = 0x204; // Allow for validating reset procedure.

    bool success = cpu_load_program(&cpu, program, size);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_UINT8(PROGRAM_START, cpu.PC);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(program, &cpu.memory->data[cpu.PC], size);
}

TEST(Cpu, LoadInvalidProgram) {
    uint8_t empty[MEMORY_SIZE] = {0};
    cpu.PC                     = 0x204; // Allow for validating reset procedure.

    // Intentionally create a program that spans entire memory range to test overflow handling.
    // Switch some bytes of the program to test memory mutation not happening on overflow.
    uint8_t program[MEMORY_SIZE] = {0};
    program[0]                   = 0xE0;
    program[256]                 = 0xA0;
    program[512]                 = 0xA0;
    program[768]                 = 0xA0;
    program[1032]                = 0xE0;

    bool success = cpu_load_program(&cpu, program, sizeof(program));
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_UINT8(0x204, cpu.PC);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(empty, &cpu.memory->data[cpu.PC], sizeof(MEMORY_SIZE - cpu.PC));
}

TEST(Cpu, RunCycle) {
    uint8_t program[2] = {0xAA, 0xBB};
    bool    success    = cpu_run_cycle(&cpu);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL(PROGRAM_START + 2, cpu.PC);
}
