#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bmp_tiny.h"

void test_bmp_tiny_types() {
    TEST(BmpTiny_Types);
    bmp_tiny_info_t info;
    memset(&info, 0, sizeof(info));
    EXPECT_TRUE(sizeof(info) > 0);
}

void test_bmp_tiny_error_values() {
    TEST(BmpTiny_ErrorValues);
    EXPECT_TRUE(BMP_TINY_OK == 0);
    EXPECT_TRUE(BMP_TINY_ERROR_FILE_OPEN != 0);
    EXPECT_TRUE(BMP_TINY_ERROR_INVALID_FORMAT != 0);
}

void test_bmp_tiny_info_fields() {
    TEST(BmpTiny_InfoFields);
    bmp_tiny_info_t info;
    memset(&info, 0, sizeof(info));
    
    info.width = 100;
    info.height = 200;
    info.bits_per_pixel = 24;
    
    EXPECT_EQ(info.width, 100);
    EXPECT_EQ(info.height, 200);
    EXPECT_EQ(info.bits_per_pixel, 24);
}

void test_bmp_tiny_write_invalid() {
    TEST(BmpTiny_WriteInvalid);
    EXPECT_TRUE(BMP_TINY_ERROR_FILE_OPEN != BMP_TINY_OK);
}

void test_bmp_tiny_read_invalid() {
    TEST(BmpTiny_ReadInvalid);
    EXPECT_TRUE(BMP_TINY_ERROR_FILE_READ != BMP_TINY_OK);
}

int main() {
    test_bmp_tiny_types();
    test_bmp_tiny_error_values();
    test_bmp_tiny_info_fields();
    test_bmp_tiny_write_invalid();
    test_bmp_tiny_read_invalid();

    return 0;
}
