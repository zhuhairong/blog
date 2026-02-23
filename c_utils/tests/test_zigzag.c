#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/zigzag.h"

void test_zigzag_encode_decode_positive() {
    TEST(Zigzag_EncodeDecodePositive);
    uint64_t encoded = zigzag_encode(5);
    int64_t decoded = zigzag_decode(encoded);
    
    EXPECT_EQ(decoded, (int64_t)5);
}

void test_zigzag_encode_decode_negative() {
    TEST(Zigzag_EncodeDecodeNegative);
    uint64_t encoded = zigzag_encode(-5);
    int64_t decoded = zigzag_decode(encoded);
    
    EXPECT_EQ(decoded, (int64_t)-5);
}

void test_zigzag_encode_decode_zero() {
    TEST(Zigzag_EncodeDecodeZero);
    uint64_t encoded = zigzag_encode(0);
    int64_t decoded = zigzag_decode(encoded);
    
    EXPECT_EQ(decoded, (int64_t)0);
    EXPECT_EQ(encoded, (uint64_t)0);
}

void test_zigzag_create_destroy() {
    TEST(Zigzag_CreateDestroy);
    zigzag_ctx_t* ctx = NULL;
    zigzag_error_t err = zigzag_create(&ctx, NULL);
    
    EXPECT_EQ(err, ZIGZAG_OK);
    EXPECT_TRUE(ctx != NULL);
    
    zigzag_destroy(ctx);
}

void test_zigzag_encode_int64() {
    TEST(Zigzag_EncodeInt64);
    zigzag_ctx_t* ctx = NULL;
    zigzag_create(&ctx, NULL);
    
    uint64_t out = 0;
    zigzag_error_t err = zigzag_encode_int64(ctx, -100, &out);
    
    EXPECT_EQ(err, ZIGZAG_OK);
    
    int64_t decoded = 0;
    err = zigzag_decode_int64(ctx, out, &decoded);
    
    EXPECT_EQ(err, ZIGZAG_OK);
    EXPECT_EQ(decoded, (int64_t)-100);
    
    zigzag_destroy(ctx);
}

int main() {
    test_zigzag_encode_decode_positive();
    test_zigzag_encode_decode_negative();
    test_zigzag_encode_decode_zero();
    test_zigzag_create_destroy();
    test_zigzag_encode_int64();

    return 0;
}
