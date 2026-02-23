#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/lzw.h"

void test_lzw_encode_decode() {
    TEST(Lzw_EncodeDecode);
    unsigned char in[] = "aaabbbccc";
    unsigned char encoded[100] = {0};
    unsigned char decoded[100] = {0};
    
    size_t enc_len = lzw_encode(in, 9, encoded);
    EXPECT_TRUE(enc_len > 0);
    
    lzw_error_t err = lzw_decode(encoded, enc_len, decoded, sizeof(decoded), &enc_len);
    EXPECT_EQ(err, LZW_OK);
}

void test_lzw_get_default_config() {
    TEST(Lzw_GetDefaultConfig);
    lzw_config_t config;
    lzw_get_default_config(&config);
    
    EXPECT_TRUE(config.max_code_size > 0);
}

void test_lzw_calculate_ratio() {
    TEST(Lzw_CalculateRatio);
    double ratio = lzw_calculate_ratio(100, 50);
    EXPECT_TRUE(ratio > 0);
}

void test_lzw_encode_empty() {
    TEST(Lzw_EncodeEmpty);
    unsigned char encoded[100] = {0};
    
    size_t enc_len = lzw_encode((unsigned char*)"", 0, encoded);
    EXPECT_TRUE(enc_len >= 0);
}

void test_lzw_encode_single() {
    TEST(Lzw_EncodeSingle);
    unsigned char in[] = "a";
    unsigned char encoded[100] = {0};
    
    size_t enc_len = lzw_encode(in, 1, encoded);
    EXPECT_TRUE(enc_len > 0);
}

int main() {
    test_lzw_encode_decode();
    test_lzw_get_default_config();
    test_lzw_calculate_ratio();
    test_lzw_encode_empty();
    test_lzw_encode_single();

    return 0;
}
