#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/endian_utils.h"

void test_endian_swap16() {
    TEST(Endian_Swap16);
    uint16_t val = 0x1234;
    uint16_t swapped = endian_swap16(val);
    EXPECT_EQ(swapped, 0x3412);
}

void test_endian_swap32() {
    TEST(Endian_Swap32);
    uint32_t val = 0x12345678;
    uint32_t swapped = endian_swap32(val);
    EXPECT_EQ(swapped, 0x78563412);
}

void test_endian_swap64() {
    TEST(Endian_Swap64);
    uint64_t val = 0x0123456789ABCDEFULL;
    uint64_t swapped = endian_swap64(val);
    EXPECT_EQ(swapped, 0xEFCDAB8967452301ULL);
}

void test_endian_is_big() {
    TEST(Endian_IsBig);
    bool is_big = endian_is_big();
    EXPECT_TRUE(is_big == true || is_big == false);
}

void test_host_to_net16() {
    TEST(Endian_HostToNet16);
    uint16_t host = 0x1234;
    uint16_t net = host_to_net16(host);
    
    if (endian_is_big()) {
        EXPECT_EQ(net, host);
    } else {
        EXPECT_EQ(net, endian_swap16(host));
    }
}

void test_host_to_net32() {
    TEST(Endian_HostToNet32);
    uint32_t host = 0x12345678;
    uint32_t net = host_to_net32(host);
    
    if (endian_is_big()) {
        EXPECT_EQ(net, host);
    } else {
        EXPECT_EQ(net, endian_swap32(host));
    }
}

void test_host_to_net64() {
    TEST(Endian_HostToNet64);
    uint64_t host = 0x0123456789ABCDEFULL;
    uint64_t net = host_to_net64(host);
    
    if (endian_is_big()) {
        EXPECT_EQ(net, host);
    } else {
        EXPECT_EQ(net, endian_swap64(host));
    }
}

void test_net_to_host16() {
    TEST(Endian_NetToHost16);
    uint16_t net = 0x1234;
    uint16_t host = net_to_host16(net);
    
    if (endian_is_big()) {
        EXPECT_EQ(host, net);
    } else {
        EXPECT_EQ(host, endian_swap16(net));
    }
}

void test_net_to_host32() {
    TEST(Endian_NetToHost32);
    uint32_t net = 0x12345678;
    uint32_t host = net_to_host32(net);
    
    if (endian_is_big()) {
        EXPECT_EQ(host, net);
    } else {
        EXPECT_EQ(host, endian_swap32(net));
    }
}

void test_net_to_host64() {
    TEST(Endian_NetToHost64);
    uint64_t net = 0x0123456789ABCDEFULL;
    uint64_t host = net_to_host64(net);
    
    if (endian_is_big()) {
        EXPECT_EQ(host, net);
    } else {
        EXPECT_EQ(host, endian_swap64(net));
    }
}

void test_endian_double_swap() {
    TEST(Endian_DoubleSwap);
    uint32_t val = 0x12345678;
    uint32_t swapped = endian_swap32(val);
    uint32_t restored = endian_swap32(swapped);
    EXPECT_EQ(restored, val);
}

void test_endian_zero() {
    TEST(Endian_Zero);
    EXPECT_EQ(endian_swap16(0), 0);
    EXPECT_EQ(endian_swap32(0), 0);
    EXPECT_EQ(endian_swap64(0), 0);
}

void test_endian_max() {
    TEST(Endian_Max);
    EXPECT_EQ(endian_swap16(0xFFFF), 0xFFFF);
    EXPECT_EQ(endian_swap32(0xFFFFFFFF), 0xFFFFFFFF);
    EXPECT_EQ(endian_swap64(0xFFFFFFFFFFFFFFFFULL), 0xFFFFFFFFFFFFFFFFULL);
}

int main() {
    test_endian_swap16();
    test_endian_swap32();
    test_endian_swap64();
    test_endian_is_big();
    test_host_to_net16();
    test_host_to_net32();
    test_host_to_net64();
    test_net_to_host16();
    test_net_to_host32();
    test_net_to_host64();
    test_endian_double_swap();
    test_endian_zero();
    test_endian_max();

    return 0;
}
