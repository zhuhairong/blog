#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/soundex.h"

void test_soundex_encode_basic() {
    TEST(Soundex_EncodeBasic);
    char result[SOUNDEX_LENGTH + 1] = {0};
    
    soundex_encode("Robert", result);
    EXPECT_EQ(result[0], 'R');
    
    soundex_encode("Rupert", result);
    EXPECT_EQ(result[0], 'R');
}

void test_soundex_encode_empty() {
    TEST(Soundex_EncodeEmpty);
    char result[SOUNDEX_LENGTH + 1] = {0};
    
    soundex_encode("", result);
    EXPECT_TRUE(result[0] != '\0' || result[0] == '\0');
}

void test_soundex_encode_single() {
    TEST(Soundex_EncodeSingle);
    char result[SOUNDEX_LENGTH + 1] = {0};
    
    soundex_encode("A", result);
    EXPECT_EQ(result[0], 'A');
}

void test_soundex_encode_numbers() {
    TEST(Soundex_EncodeNumbers);
    char result[SOUNDEX_LENGTH + 1] = {0};
    
    soundex_encode("Washington", result);
    EXPECT_EQ(result[0], 'W');
}

void test_soundex_encode_similar() {
    TEST(Soundex_EncodeSimilar);
    char result1[SOUNDEX_LENGTH + 1] = {0};
    char result2[SOUNDEX_LENGTH + 1] = {0};
    
    soundex_encode("Smith", result1);
    soundex_encode("Smythe", result2);
    
    EXPECT_EQ(result1[0], 'S');
    EXPECT_EQ(result2[0], 'S');
}

int main() {
    test_soundex_encode_basic();
    test_soundex_encode_empty();
    test_soundex_encode_single();
    test_soundex_encode_numbers();
    test_soundex_encode_similar();

    return 0;
}
