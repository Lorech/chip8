#include <stdbool.h>

#include "memory.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_test_execution(void) {
    TEST_ASSERT_TRUE(true);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_test_execution);
    return UNITY_END();
}
