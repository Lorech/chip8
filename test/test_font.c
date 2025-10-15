#include "font.h"
#include "unity_fixture.h"

TEST_GROUP(Font);

TEST_SETUP(Font) {}

TEST_TEAR_DOWN(Font) {}

TEST(Font, GetChip48) {
    font_data_t font = font_get(FONT_CHIP48);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    uint8_t one_expected[5]     = {0x20, 0x60, 0x20, 0x20, 0x70};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetCosmacVip) {
    font_data_t font = font_get(FONT_COSMACVIP);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    uint8_t one_expected[5]     = {0x60, 0x20, 0x20, 0x20, 0x70};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetDream6800) {
    font_data_t font = font_get(FONT_DREAM6800);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    uint8_t one_expected[5]     = {0x40, 0x40, 0x40, 0x40, 0x40};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetEti660) {
    font_data_t font = font_get(FONT_ETI660);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    uint8_t one_expected[5]     = {0x20, 0x20, 0x20, 0x20, 0x20};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetInvalid) {
    font_data_t font = font_get(FONT_COUNT);
    TEST_ASSERT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(0, font.size);
}
