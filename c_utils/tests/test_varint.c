#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/varint.h"

void test_varint_encode_decode_small() {
    TEST(Varint_EncodeDecodeSmall);
    uint8_t buf[10] = {0};
    uint64_t val = 127;
    
    size_t len = varint_encode(val, buf);
    EXPECT_TRUE(len > 0);
    
    uint64_t decoded = 0;
    size_t decoded_len = varint_decode(buf, &decoded);
    
    EXPECT_TRUE(decoded_len > 0);
    EXPECT_EQ(decoded, val);
}

void test_varint_encode_decode_large() {
    TEST(Varint_EncodeDecodeLarge);
    uint8_t buf[10] = {0};
    uint64_t val = 1234567890;
    
    size_t len = varint_encode(val, buf);
    EXPECT_TRUE(len > 0);
    
    uint64_t decoded = 0;
    size_t decoded_len = varint_decode(buf, &decoded);
    
    EXPECT_TRUE(decoded_len > 0);
    EXPECT_EQ(decoded, val);
}

void test_varint_encode_decode_zero() {
    TEST(Varint_EncodeDecodeZero);
    uint8_t buf[10] = {0};
    uint64_t val = 0;
    
    size_t len = varint_encode(val, buf);
    EXPECT_TRUE(len > 0);
    
    uint64_t decoded = 0;
    size_t decoded_len = varint_decode(buf, &decoded);
    
    EXPECT_TRUE(decoded_len > 0);
    EXPECT_EQ(decoded, val);
}

void test_varint_encode_decode_max() {
    TEST(Varint_EncodeDecodeMax);
    uint8_t buf[10] = {0};
    uint64_t val = UINT64_MAX;
    
    size_t len = varint_encode(val, buf);
    EXPECT_TRUE(len > 0);
    
    uint64_t decoded = 0;
    size_t decoded_len = varint_decode(buf, &decoded);
    
    EXPECT_TRUE(decoded_len > 0);
    EXPECT_EQ(decoded, val);
}

void test_varint_encode_size() {
    TEST(Varint_EncodeSize);
    uint8_t buf[10] = {0};
    
    size_t len1 = varint_encode(0, buf);
    size_t len2 = varint_encode(127, buf);
    size_t len3 = varint_encode(128, buf);
    
    EXPECT_TRUE(len1 > 0);
    EXPECT_TRUE(len2 > 0);
    EXPECT_TRUE(len3 > 0);
}

int main() {
    test_varint_encode_decode_small();
    test_varint_encode_decode_large();
    test_varint_encode_decode_zero();
    test_varint_encode_decode_max();
    test_varint_encode_size();

    return 0;
}
