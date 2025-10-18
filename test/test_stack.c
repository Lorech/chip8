#include <stdbool.h>

#include "stack.h"
#include "unity_fixture.h"

TEST_GROUP(Stack);

static chipstack_t stack;

TEST_SETUP(Stack) {
    stack = stack_create(1);
}

TEST_TEAR_DOWN(Stack) {
    stack_destroy(&stack);
}

TEST(Stack, PushPop) {
    TEST_ASSERT_EQUAL_UINT8(1, stack.size);
    TEST_ASSERT_EQUAL_UINT8(0, stack.top);

    uint16_t push_address = 0x200;
    bool     push_success = stack_push(&stack, push_address);
    TEST_ASSERT_TRUE(push_success);
    TEST_ASSERT_EQUAL_UINT8(1, stack.top);

    uint16_t pop_address;
    bool     pop_success = stack_pop(&stack, &pop_address);
    TEST_ASSERT_TRUE(pop_success);
    TEST_ASSERT_EQUAL_UINT8(0, stack.top);
    TEST_ASSERT_EQUAL_UINT16(push_address, pop_address);
}

TEST(Stack, LimitsSize) {
    uint16_t pop_address = 0x1234;
    bool     pop_success = stack_pop(&stack, &pop_address);
    TEST_ASSERT_FALSE(pop_success);
    TEST_ASSERT_EQUAL_UINT8(0x1234, pop_address); // Should remain unchanged.

    // Valid push to reach the top of the stack.
    uint16_t push_address       = 0x200;
    bool     first_push_success = stack_push(&stack, push_address);
    TEST_ASSERT_TRUE(first_push_success);
    TEST_ASSERT_EQUAL_UINT8(1, stack.top);

    bool second_push_success = stack_push(&stack, push_address);
    TEST_ASSERT_FALSE(second_push_success);
    TEST_ASSERT_EQUAL_UINT8(1, stack.top);
}
