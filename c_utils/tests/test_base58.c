#include "../c_utils/utest.h"
#include "../c_utils/base58.h"
#include <string.h>
#include <stdlib.h>

void test_base58_encode_size() {
    TEST(Base58_EncodeSize);
    EXPECT_TRUE(base58_encode_size(0) >= 1);
    EXPECT_TRUE(base58_encode_size(10) >= 14);
}

void test_base58_encode() {
    TEST(Base58_Encode);
    unsigned char input[] = "Hello";
    char output[32];
    size_t out_len = sizeof(output);
    bool result = base58_encode(input, 5, output, &out_len);
    EXPECT_TRUE(result);
    EXPECT_TRUE(out_len > 0);
}

void test_base58_decode() {
    TEST(Base58_Decode);
    unsigned char input[] = "Hello";
    char encoded[32];
    size_t enc_len = sizeof(encoded);
    base58_encode(input, 5, encoded, &enc_len);
    
    unsigned char decoded[32];
    size_t dec_len = sizeof(decoded);
    bool result = base58_decode(encoded, decoded, &dec_len);
    EXPECT_TRUE(result);
    EXPECT_TRUE(dec_len > 0);
    decoded[dec_len] = '\0';
    EXPECT_STR_EQ((char*)decoded, "Hello");
}

void test_base58_roundtrip() {
    TEST(Base58_Roundtrip);
    unsigned char input[] = "The quick brown fox jumps over the lazy dog";
    size_t input_len = strlen((char*)input);
    
    char encoded[128];
    size_t enc_len = sizeof(encoded);
    bool enc_result = base58_encode(input, input_len, encoded, &enc_len);
    EXPECT_TRUE(enc_result);
    
    unsigned char decoded[128];
    size_t dec_len = sizeof(decoded);
    bool dec_result = base58_decode(encoded, decoded, &dec_len);
    
    EXPECT_TRUE(dec_result);
    EXPECT_EQ(dec_len, input_len);
    EXPECT_TRUE(memcmp(input, decoded, input_len) == 0);
}

void test_base58_empty() {
    TEST(Base58_Empty);
    char encoded[8];
    size_t out_len = sizeof(encoded);
    base58_encode((unsigned char*)"", 0, encoded, &out_len);
}

void test_base58_is_valid() {
    TEST(Base58_IsValid);
    EXPECT_TRUE(base58_is_valid("2NEpo7TZRRr"));
    EXPECT_TRUE(!base58_is_valid("2NEpo7TZRR0"));
    EXPECT_TRUE(!base58_is_valid("2NEpo7TZRRl"));
    EXPECT_TRUE(!base58_is_valid("2NEpo7TZRRO"));
}

int main() {
    UTEST_BEGIN();
    test_base58_encode_size();
    test_base58_encode();
    test_base58_decode();
    test_base58_roundtrip();
    test_base58_empty();
    test_base58_is_valid();
    UTEST_END();
}
