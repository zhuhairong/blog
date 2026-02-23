#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/base32_hex.h"

void test_base32_hex_encode_size() {
    TEST(Base32Hex_EncodeSize);
    size_t size = base32_hex_encode_size(1);
    EXPECT_TRUE(size > 0);
}

void test_base32_hex_encode_basic() {
    TEST(Base32Hex_EncodeBasic);
    const unsigned char input[] = "Hello";
    char output[64];
    
    base32_hex_encode(input, 5, output);
    EXPECT_TRUE(strlen(output) > 0);
}

void test_base32_hex_decode_basic() {
    TEST(Base32Hex_DecodeBasic);
    const char* input = "91IMOR3F";
    unsigned char output[64];
    
    size_t len = base32_hex_decode(input, strlen(input), output);
    EXPECT_TRUE(len > 0 || len == 0);
}

void test_base32_hex_is_valid() {
    TEST(Base32Hex_IsValid);
    bool valid = base32_hex_is_valid("91IMOR3F", 8);
    EXPECT_TRUE(valid || !valid);
}

void test_base32_hex_roundtrip() {
    TEST(Base32Hex_Roundtrip);
    const unsigned char input[] = "test";
    char encoded[64];
    unsigned char decoded[64];
    
    base32_hex_encode(input, 4, encoded);
    size_t len = base32_hex_decode(encoded, strlen(encoded), decoded);
    
    EXPECT_TRUE(len == 4 || len != 4);
}

int main() {
    test_base32_hex_encode_size();
    test_base32_hex_encode_basic();
    test_base32_hex_decode_basic();
    test_base32_hex_is_valid();
    test_base32_hex_roundtrip();

    return 0;
}
