#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bitstream.h"

void test_bitstream_init() {
    TEST(Bitstream_Init);
    uint8_t data[] = {0xFF, 0x00, 0xAA};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    EXPECT_TRUE(bs.buf == data);
    EXPECT_EQ(bs.len, sizeof(data));
    EXPECT_EQ(bs.pos, (size_t)0);
}

void test_bitstream_init_null() {
    TEST(Bitstream_InitNull);
    bitstream_t bs;
    bitstream_init(&bs, NULL, 0);
    EXPECT_TRUE(bs.buf == NULL);
    EXPECT_EQ(bs.len, (size_t)0);
    EXPECT_EQ(bs.pos, (size_t)0);
}

void test_bitstream_read_single_bits() {
    TEST(Bitstream_ReadSingleBits);
    uint8_t data[] = {0b10101010};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)1);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)0);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)1);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)0);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)1);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)0);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)1);
    EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)0);
}

void test_bitstream_read_multiple_bits() {
    TEST(Bitstream_ReadMultipleBits);
    uint8_t data[] = {0x12, 0x34};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 4), (uint32_t)0x1);
    EXPECT_EQ(bitstream_read(&bs, 4), (uint32_t)0x2);
    EXPECT_EQ(bitstream_read(&bs, 4), (uint32_t)0x3);
    EXPECT_EQ(bitstream_read(&bs, 4), (uint32_t)0x4);
}

void test_bitstream_read_byte() {
    TEST(Bitstream_ReadByte);
    uint8_t data[] = {0xAB, 0xCD, 0xEF};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 8), (uint32_t)0xAB);
    EXPECT_EQ(bitstream_read(&bs, 8), (uint32_t)0xCD);
    EXPECT_EQ(bitstream_read(&bs, 8), (uint32_t)0xEF);
}

void test_bitstream_read_cross_byte() {
    TEST(Bitstream_ReadCrossByte);
    uint8_t data[] = {0xAB, 0xCD};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    bitstream_read(&bs, 4);
    EXPECT_EQ(bitstream_read(&bs, 8), (uint32_t)0xBC);
    EXPECT_EQ(bitstream_read(&bs, 4), (uint32_t)0xD);
}

void test_bitstream_read_large_bits() {
    TEST(Bitstream_ReadLargeBits);
    uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 16), (uint32_t)0x1234);
    EXPECT_EQ(bitstream_read(&bs, 16), (uint32_t)0x5678);
}

void test_bitstream_read_zero_bits() {
    TEST(Bitstream_ReadZeroBits);
    uint8_t data[] = {0xFF};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 0), (uint32_t)0);
    EXPECT_EQ(bs.pos, (size_t)0);
}

void test_bitstream_read_all_zeros() {
    TEST(Bitstream_ReadAllZeros);
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)0);
    }
}

void test_bitstream_read_all_ones() {
    TEST(Bitstream_ReadAllOnes);
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(bitstream_read(&bs, 1), (uint32_t)1);
    }
}

void test_bitstream_position_tracking() {
    TEST(Bitstream_PositionTracking);
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bs.pos, (size_t)0);
    bitstream_read(&bs, 1);
    EXPECT_EQ(bs.pos, (size_t)1);
    bitstream_read(&bs, 7);
    EXPECT_EQ(bs.pos, (size_t)8);
    bitstream_read(&bs, 16);
    EXPECT_EQ(bs.pos, (size_t)24);
}

void test_bitstream_stability_sequential_reads() {
    TEST(Bitstream_StabilitySequentialReads);
    uint8_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (uint8_t)i;
    }
    
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    for (int i = 0; i < 256; i++) {
        uint32_t val = bitstream_read(&bs, 8);
        EXPECT_EQ(val, (uint32_t)i);
    }
}

void test_bitstream_stability_random_width_reads() {
    TEST(Bitstream_StabilityRandomWidthReads);
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    int widths[] = {3, 5, 7, 2, 8, 4, 6, 1, 9, 11, 6};
    int total_bits = 0;
    for (int i = 0; i < 11; i++) {
        bitstream_read(&bs, widths[i]);
        total_bits += widths[i];
        EXPECT_EQ(bs.pos, (size_t)total_bits);
    }
}

void test_bitstream_edge_case_single_byte() {
    TEST(Bitstream_EdgeCaseSingleByte);
    uint8_t data[] = {0x55};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 8), (uint32_t)0x55);
}

void test_bitstream_edge_case_32bits() {
    TEST(Bitstream_EdgeCase32Bits);
    uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));
    
    EXPECT_EQ(bitstream_read(&bs, 32), (uint32_t)0x12345678);
}

int main() {
    test_bitstream_init();
    test_bitstream_init_null();
    test_bitstream_read_single_bits();
    test_bitstream_read_multiple_bits();
    test_bitstream_read_byte();
    test_bitstream_read_cross_byte();
    test_bitstream_read_large_bits();
    test_bitstream_read_zero_bits();
    test_bitstream_read_all_zeros();
    test_bitstream_read_all_ones();
    test_bitstream_position_tracking();
    test_bitstream_stability_sequential_reads();
    test_bitstream_stability_random_width_reads();
    test_bitstream_edge_case_single_byte();
    test_bitstream_edge_case_32bits();

    return 0;
}
