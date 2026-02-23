#include "../c_utils/utest.h"
#include "../c_utils/base16.h"
#include <string.h>
#include <stdlib.h>

void test_base16_encode_size() {
    TEST(Base16_EncodeSize);
    EXPECT_EQ(base16_encode_size(0), 1);
    EXPECT_EQ(base16_encode_size(1), 3);
    EXPECT_EQ(base16_encode_size(2), 5);
    EXPECT_EQ(base16_encode_size(4), 9);
}

void test_base16_encode() {
    TEST(Base16_Encode);
    unsigned char input[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    char output[16];
    base16_encode(input, 5, output, false);
    EXPECT_STR_EQ(output, "48656c6c6f");
}

void test_base16_encode_upper() {
    TEST(Base16_EncodeUpper);
    unsigned char input[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
    char output[16];
    base16_encode(input, 5, output, true);
    EXPECT_STR_EQ(output, "48656C6C6F");
}

void test_base16_decode() {
    TEST(Base16_Decode);
    char input[] = "48656c6c6f";
    unsigned char output[8];
    size_t len = base16_decode(input, 10, output);
    EXPECT_EQ(len, 5);
    EXPECT_STR_EQ((char*)output, "Hello");
}

void test_base16_roundtrip() {
    TEST(Base16_Roundtrip);
    unsigned char input[] = {0x00, 0x01, 0x02, 0x7f, 0x80, 0xfe, 0xff};
    char encoded[20];
    base16_encode(input, 7, encoded, false);
    
    unsigned char decoded[8];
    size_t len = base16_decode(encoded, strlen(encoded), decoded);
    
    EXPECT_EQ(len, 7);
    EXPECT_TRUE(memcmp(input, decoded, 7) == 0);
}

void test_base16_empty() {
    TEST(Base16_Empty);
    char encoded[8];
    base16_encode((unsigned char*)"", 0, encoded, false);
    EXPECT_STR_EQ(encoded, "");
    
    unsigned char decoded[8];
    size_t len = base16_decode("", 0, decoded);
    EXPECT_EQ(len, 0);
}

void test_base16_is_valid() {
    TEST(Base16_IsValid);
    EXPECT_TRUE(base16_is_valid("48656c6c6f", 10));
    EXPECT_TRUE(base16_is_valid("48656C6C6F", 10));
    EXPECT_TRUE(!base16_is_valid("48656g6c6f", 10));
    EXPECT_TRUE(!base16_is_valid("4865 6c6f", 9));
}

int main() {
    UTEST_BEGIN();
    test_base16_encode_size();
    test_base16_encode();
    test_base16_encode_upper();
    test_base16_decode();
    test_base16_roundtrip();
    test_base16_empty();
    test_base16_is_valid();
    UTEST_END();
}
