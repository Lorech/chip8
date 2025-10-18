#include "timer.h"
#include "unity_fixture.h"

TEST_GROUP(Timer);

static timer_t timer;

TEST_SETUP(Timer) {
    timer_init(&timer);
}

TEST_TEAR_DOWN(Timer) {}

TEST(Timer, Ticks) {
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.delay, "Delay timer should initialize to 0");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.sound, "Sound timer should initialize to 0");

    timer.delay = 1;
    timer.sound = 1;
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, timer.delay, "Delay timer should be writable");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, timer.sound, "Sound timer should be writable");

    timer_tick(&timer);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.delay, "Delay timer should tick down");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.sound, "Sound timer should tick down");

    timer_tick(&timer);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.delay, "Delay timer shouldn't tick below 0");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, timer.sound, "Sound timer shouldn't tick below 0");
}
