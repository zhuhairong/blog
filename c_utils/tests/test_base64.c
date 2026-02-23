#include "../c_utils/utest.h"
#include "../c_utils/base64.h"
#include <string.h>
#include <stdlib.h>

void test_base64_encode_size() {
    TEST(Base64_EncodeSize);
    EXPECT_EQ(base64_encode_size(0), 1);
    EXPECT_EQ(base64_encode_size(1), 5);
    EXPECT_EQ(base64_encode_size(2), 5);
    EXPECT_EQ(base64_encode_size(3), 5);
    EXPECT_EQ(base64_encode_size(4), 9);
    EXPECT_EQ(base64_encode_size(6), 9);
}

void test_base64_decode_size() {
    TEST(Base64_DecodeSize);
    EXPECT_EQ(base64_decode_size(4), 3);
    EXPECT_EQ(base64_decode_size(8), 6);
    EXPECT_EQ(base64_decode_size(12), 9);
}

void test_base64_encode() {
    TEST(Base64_Encode);
    unsigned char input[] = "Hello";
    char output[16];
    base64_encode(input, 5, output);
    EXPECT_STR_EQ(output, "SGVsbG8=");
}

void test_base64_encode_alloc() {
    TEST(Base64_EncodeAlloc);
    unsigned char input[] = "Hello";
    char* output = base64_encode_alloc(input, 5);
    EXPECT_TRUE(output != NULL);
    EXPECT_STR_EQ(output, "SGVsbG8=");
    free(output);
}

void test_base64_decode() {
    TEST(Base64_Decode);
    char input[] = "SGVsbG8=";
    unsigned char output[16];
    size_t len = base64_decode(input, 8, output);
    EXPECT_EQ(len, 5);
    output[len] = '\0';
    EXPECT_STR_EQ((char*)output, "Hello");
}

void test_base64_decode_alloc() {
    TEST(Base64_DecodeAlloc);
    char input[] = "SGVsbG8=";
    size_t out_len;
    unsigned char* output = base64_decode_alloc(input, 8, &out_len);
    EXPECT_TRUE(output != NULL);
    EXPECT_EQ(out_len, 5);
    output[out_len] = '\0';
    EXPECT_STR_EQ((char*)output, "Hello");
    free(output);
}

void test_base64_roundtrip() {
    TEST(Base64_Roundtrip);
    unsigned char input[] = "The quick brown fox jumps over the lazy dog";
    size_t input_len = strlen((char*)input);
    
    char encoded[64];
    base64_encode(input, input_len, encoded);
    
    unsigned char decoded[64];
    size_t decoded_len = base64_decode(encoded, strlen(encoded), decoded);
    
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_TRUE(memcmp(input, decoded, input_len) == 0);
}

void test_base64_url_encode() {
    TEST(Base64_UrlEncode);
    unsigned char input[] = "\xff\xfe";
    char output[16];
    base64_url_encode(input, 2, output);
    EXPECT_TRUE(output[0] != '+');
    EXPECT_TRUE(output[0] != '/');
}

void test_base64_url_decode() {
    TEST(Base64_UrlDecode);
    unsigned char input[] = "\xff\xfe";
    char encoded[16];
    base64_url_encode(input, 2, encoded);
    
    unsigned char decoded[16];
    size_t len = base64_url_decode(encoded, strlen(encoded), decoded);
    EXPECT_EQ(len, 2);
    EXPECT_TRUE(memcmp(input, decoded, 2) == 0);
}

void test_base64_is_valid() {
    TEST(Base64_IsValid);
    EXPECT_TRUE(base64_is_valid("SGVsbG8=", 8));
    EXPECT_TRUE(!base64_is_valid("SGVs bG8=", 8));
    EXPECT_TRUE(!base64_is_valid("SGVs!G8=", 8));
}

void test_base64_empty() {
    TEST(Base64_Empty);
    char encoded[8];
    base64_encode((unsigned char*)"", 0, encoded);
    EXPECT_STR_EQ(encoded, "");
    
    size_t len = base64_decode("", 0, (unsigned char*)encoded);
    EXPECT_EQ(len, 0);
}

void test_base64_binary() {
    TEST(Base64_Binary);
    unsigned char input[256];
    for (int i = 0; i < 256; i++) input[i] = (unsigned char)i;
    
    char encoded[400];
    base64_encode(input, 256, encoded);
    
    unsigned char decoded[256];
    size_t len = base64_decode(encoded, strlen(encoded), decoded);
    
    EXPECT_EQ(len, 256);
    EXPECT_TRUE(memcmp(input, decoded, 256) == 0);
}

int main() {
    UTEST_BEGIN();
    test_base64_encode_size();
    test_base64_decode_size();
    test_base64_encode();
    test_base64_encode_alloc();
    test_base64_decode();
    test_base64_decode_alloc();
    test_base64_roundtrip();
    test_base64_url_encode();
    test_base64_url_decode();
    test_base64_is_valid();
    test_base64_empty();
    test_base64_binary();
    UTEST_END();
}
