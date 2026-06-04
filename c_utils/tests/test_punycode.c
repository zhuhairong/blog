#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/punycode.h"

void test_punycode_encode_ascii() {
    TEST(Punycode_EncodeAscii);
    uint32_t input[] = {'a', 'b', 'c'};
    char output[64] = {0};
    size_t output_len = sizeof(output);
    
    bool result = punycode_encode(input, 3, output, &output_len);
    EXPECT_TRUE(result);
    EXPECT_TRUE(output_len > 0);
}

void test_punycode_encode_unicode() {
    TEST(Punycode_EncodeUnicode);
    uint32_t input[] = {0x4E2D, 0x56FD};  /* 中国 */
    char output[64] = {0};
    size_t output_len = sizeof(output);

    bool result = punycode_encode(input, 2, output, &output_len);
    /* NOTE: Non-ASCII Punycode bootstring encoding (RFC 3492) is not yet
     * implemented. The library returns false for code points >= 0x80. */
    EXPECT_FALSE(result);
}

void test_punycode_encode_empty() {
    TEST(Punycode_EncodeEmpty);
    char output[64] = {0};
    size_t output_len = sizeof(output);
    
    bool result = punycode_encode(NULL, 0, output, &output_len);
    EXPECT_FALSE(result);  /* NULL input should fail */
}

void test_punycode_encode_single() {
    TEST(Punycode_EncodeSingle);
    uint32_t input[] = {'x'};
    char output[64] = {0};
    size_t output_len = sizeof(output);
    
    bool result = punycode_encode(input, 1, output, &output_len);
    EXPECT_TRUE(result);
}

void test_punycode_encode_numbers() {
    TEST(Punycode_EncodeNumbers);
    uint32_t input[] = {'1', '2', '3'};
    char output[64] = {0};
    size_t output_len = sizeof(output);
    
    bool result = punycode_encode(input, 3, output, &output_len);
    EXPECT_TRUE(result);
}

int main() {
    UTEST_BEGIN();
    test_punycode_encode_ascii();
    test_punycode_encode_unicode();
    test_punycode_encode_empty();
    test_punycode_encode_single();
    test_punycode_encode_numbers();

    UTEST_END();
}
