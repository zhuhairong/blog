#include "../c_utils/utest.h"
#include "../c_utils/base32.h"
#include <string.h>
#include <stdlib.h>

void test_base32_encode_size() {
    TEST(Base32_EncodeSize);
    EXPECT_EQ(base32_encode_size(0), 1);
    EXPECT_EQ(base32_encode_size(1), 9);
    EXPECT_EQ(base32_encode_size(5), 9);
    EXPECT_EQ(base32_encode_size(10), 17);
}

void test_base32_encode() {
    TEST(Base32_Encode);
    unsigned char input[] = "Hello";
    char output[16];
    base32_encode(input, 5, output);
    EXPECT_STR_EQ(output, "JBSWY3DP");
}

void test_base32_decode() {
    TEST(Base32_Decode);
    char input[] = "JBSWY3DP";
    unsigned char output[16];
    size_t len = base32_decode(input, 8, output);
    EXPECT_EQ(len, 5);
    output[len] = '\0';
    EXPECT_STR_EQ((char*)output, "Hello");
}

void test_base32_roundtrip() {
    TEST(Base32_Roundtrip);
    unsigned char input[] = "The quick brown fox";
    size_t input_len = strlen((char*)input);
    
    char encoded[64];
    base32_encode(input, input_len, encoded);
    
    unsigned char decoded[64];
    size_t decoded_len = base32_decode(encoded, strlen(encoded), decoded);
    
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_TRUE(memcmp(input, decoded, input_len) == 0);
}

void test_base32_empty() {
    TEST(Base32_Empty);
    char encoded[8];
    base32_encode((unsigned char*)"", 0, encoded);
    EXPECT_STR_EQ(encoded, "");
    
    unsigned char decoded[8];
    size_t len = base32_decode("", 0, decoded);
    EXPECT_EQ(len, 0);
}

void test_base32_is_valid() {
    TEST(Base32_IsValid);
    EXPECT_TRUE(base32_is_valid("JBSWY3DP", 8));
    EXPECT_TRUE(!base32_is_valid("JBSWY3D!", 8));
}

int main() {
    UTEST_BEGIN();
    test_base32_encode_size();
    test_base32_encode();
    test_base32_decode();
    test_base32_roundtrip();
    test_base32_empty();
    test_base32_is_valid();
    UTEST_END();
}
