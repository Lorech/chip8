#include <string.h>

#include "font.h"
#include "unity_fixture.h"

TEST_GROUP(Font);

TEST_SETUP(Font) {}

TEST_TEAR_DOWN(Font) {}

TEST(Font, GetChip48) {
    font_data_t font = font_get(FONT_CHIP48);
    TEST_ASSERT_NOT_NULL(font.name);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    char   *name_expected        = "CHIP-48";
    uint8_t name_length_expected = strlen(name_expected);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(name_expected, font.name, name_length_expected);

    uint8_t one_expected[5]     = {0x20, 0x60, 0x20, 0x20, 0x70};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetCosmacVip) {
    font_data_t font = font_get(FONT_COSMACVIP);
    TEST_ASSERT_NOT_NULL(font.name);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    char   *name_expected        = "Cosmac VIP";
    uint8_t name_length_expected = strlen(name_expected);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(name_expected, font.name, name_length_expected);

    uint8_t one_expected[5]     = {0x60, 0x20, 0x20, 0x20, 0x70};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetDream6800) {
    font_data_t font = font_get(FONT_DREAM6800);
    TEST_ASSERT_NOT_NULL(font.name);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    char   *name_expected        = "Dream 6800";
    uint8_t name_length_expected = strlen(name_expected);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(name_expected, font.name, name_length_expected);

    uint8_t one_expected[5]     = {0x40, 0x40, 0x40, 0x40, 0x40};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetEti660) {
    font_data_t font = font_get(FONT_ETI660);
    TEST_ASSERT_NOT_NULL(font.name);
    TEST_ASSERT_NOT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(80, font.size);

    char   *name_expected        = "ETI 660";
    uint8_t name_length_expected = strlen(name_expected);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(name_expected, font.name, name_length_expected);

    uint8_t one_expected[5]     = {0x20, 0x20, 0x20, 0x20, 0x20};
    uint8_t one_length_expected = sizeof(one_expected);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(one_expected, &font.data[5], one_length_expected);
}

TEST(Font, GetInvalid) {
    font_data_t font = font_get(FONT_COUNT);
    TEST_ASSERT_NULL(font.name);
    TEST_ASSERT_NULL(font.data);
    TEST_ASSERT_EQUAL_UINT8(0, font.size);
}

TEST(Font, ByName) {
    font_type_t chip_expected = font_by_name("CHIP-48");
    TEST_ASSERT_EQUAL_UINT8(chip_expected, FONT_CHIP48);

    font_type_t cosmac_expected = font_by_name("Cosmac VIP");
    TEST_ASSERT_EQUAL_UINT8(cosmac_expected, FONT_COSMACVIP);

    font_type_t dream_expected = font_by_name("Dream 6800");
    TEST_ASSERT_EQUAL_UINT8(dream_expected, FONT_DREAM6800);

    font_type_t eti_expected = font_by_name("ETI 660");
    TEST_ASSERT_EQUAL_UINT8(eti_expected, FONT_ETI660);

    font_type_t invalid_expected = font_by_name("Invalid");
    TEST_ASSERT_EQUAL_UINT8(invalid_expected, FONT_COUNT);
}
