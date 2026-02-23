#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/rle.h"

void test_rle_encode_basic() {
    TEST(Rle_EncodeBasic);
    unsigned char in[] = {1, 1, 1, 2, 2, 3, 3, 3, 3};
    unsigned char out[20] = {0};
    
    size_t out_len = rle_encode(in, sizeof(in), out);
    EXPECT_TRUE(out_len > 0);
    EXPECT_TRUE(out_len < sizeof(in));
}

void test_rle_decode_basic() {
    TEST(Rle_DecodeBasic);
    unsigned char in[] = {1, 1, 1, 2, 2, 3, 3, 3, 3};
    unsigned char encoded[20] = {0};
    unsigned char decoded[20] = {0};
    
    size_t enc_len = rle_encode(in, sizeof(in), encoded);
    EXPECT_TRUE(enc_len > 0);
    
    size_t dec_len = rle_decode(encoded, enc_len, decoded);
    EXPECT_EQ(dec_len, sizeof(in));
    
    EXPECT_TRUE(memcmp(in, decoded, sizeof(in)) == 0);
}

void test_rle_encode_no_repeat() {
    TEST(Rle_EncodeNoRepeat);
    unsigned char in[] = {1, 2, 3, 4, 5};
    unsigned char out[20] = {0};
    
    size_t out_len = rle_encode(in, sizeof(in), out);
    EXPECT_TRUE(out_len > 0);
}

void test_rle_encode_single() {
    TEST(Rle_EncodeSingle);
    unsigned char in[] = {5};
    unsigned char out[10] = {0};
    
    size_t out_len = rle_encode(in, sizeof(in), out);
    EXPECT_TRUE(out_len > 0);
}

void test_rle_encode_all_same() {
    TEST(Rle_EncodeAllSame);
    unsigned char in[] = {7, 7, 7, 7, 7, 7, 7, 7};
    unsigned char out[20] = {0};
    
    size_t out_len = rle_encode(in, sizeof(in), out);
    EXPECT_TRUE(out_len > 0);
    EXPECT_TRUE(out_len < sizeof(in));
}

int main() {
    test_rle_encode_basic();
    test_rle_decode_basic();
    test_rle_encode_no_repeat();
    test_rle_encode_single();
    test_rle_encode_all_same();

    return 0;
}
