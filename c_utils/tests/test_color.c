#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/color.h"

void test_rgb_to_hsl() {
    TEST(Color_RgbToHsl);
    rgb_t rgb = {255, 0, 0};
    hsl_t hsl = rgb_to_hsl(rgb);
    
    EXPECT_TRUE(fabs(hsl.h - 0.0) < 1.0);
    EXPECT_TRUE(fabs(hsl.s - 1.0) < 0.01);
    EXPECT_TRUE(fabs(hsl.l - 0.5) < 0.01);
}

void test_hsl_to_rgb() {
    TEST(Color_HslToRgb);
    hsl_t hsl = {0.0, 1.0, 0.5};
    rgb_t rgb = hsl_to_rgb(hsl);
    
    EXPECT_EQ(rgb.r, 255);
    EXPECT_EQ(rgb.g, 0);
    EXPECT_EQ(rgb.b, 0);
}

void test_rgb_to_hsl_green() {
    TEST(Color_RgbToHslGreen);
    rgb_t rgb = {0, 255, 0};
    hsl_t hsl = rgb_to_hsl(rgb);
    
    EXPECT_TRUE(fabs(hsl.h - 120.0) < 1.0);
    EXPECT_TRUE(fabs(hsl.s - 1.0) < 0.01);
    EXPECT_TRUE(fabs(hsl.l - 0.5) < 0.01);
}

void test_rgb_to_hsl_blue() {
    TEST(Color_RgbToHslBlue);
    rgb_t rgb = {0, 0, 255};
    hsl_t hsl = rgb_to_hsl(rgb);
    
    EXPECT_TRUE(fabs(hsl.h - 240.0) < 1.0);
    EXPECT_TRUE(fabs(hsl.s - 1.0) < 0.01);
    EXPECT_TRUE(fabs(hsl.l - 0.5) < 0.01);
}

void test_hsl_to_rgb_green() {
    TEST(Color_HslToRgbGreen);
    hsl_t hsl = {120.0, 1.0, 0.5};
    rgb_t rgb = hsl_to_rgb(hsl);
    
    EXPECT_EQ(rgb.r, 0);
    EXPECT_EQ(rgb.g, 255);
    EXPECT_EQ(rgb.b, 0);
}

void test_hsl_to_rgb_blue() {
    TEST(Color_HslToRgbBlue);
    hsl_t hsl = {240.0, 1.0, 0.5};
    rgb_t rgb = hsl_to_rgb(hsl);
    
    EXPECT_EQ(rgb.r, 0);
    EXPECT_EQ(rgb.g, 0);
    EXPECT_EQ(rgb.b, 255);
}

void test_rgb_to_hsl_white() {
    TEST(Color_RgbToHslWhite);
    rgb_t rgb = {255, 255, 255};
    hsl_t hsl = rgb_to_hsl(rgb);
    
    EXPECT_TRUE(fabs(hsl.l - 1.0) < 0.01);
}

void test_rgb_to_hsl_black() {
    TEST(Color_RgbToHslBlack);
    rgb_t rgb = {0, 0, 0};
    hsl_t hsl = rgb_to_hsl(rgb);
    
    EXPECT_TRUE(fabs(hsl.l - 0.0) < 0.01);
}

int main() {
    test_rgb_to_hsl();
    test_hsl_to_rgb();
    test_rgb_to_hsl_green();
    test_rgb_to_hsl_blue();
    test_hsl_to_rgb_green();
    test_hsl_to_rgb_blue();
    test_rgb_to_hsl_white();
    test_rgb_to_hsl_black();

    return 0;
}
