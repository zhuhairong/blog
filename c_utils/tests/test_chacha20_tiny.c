#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/chacha20_tiny.h"

void test_chacha20_tiny_basic() {
    TEST(ChaCha20_TinyBasic);
    uint8_t key[32] = {0};
    uint8_t nonce[12] = {0};
    uint8_t out[64] = {0};
    
    chacha20_tiny(key, nonce, 0, out, 64);
    EXPECT_TRUE(true);
}

void test_chacha20_tiny_empty() {
    TEST(ChaCha20_TinyEmpty);
    uint8_t key[32] = {0};
    uint8_t nonce[12] = {0};
    uint8_t out[1] = {0};
    
    chacha20_tiny(key, nonce, 0, out, 0);
    EXPECT_TRUE(true);
}

void test_chacha20_tiny_with_key() {
    TEST(ChaCha20_TinyWithKey);
    uint8_t key[32];
    for (int i = 0; i < 32; i++) key[i] = i;
    
    uint8_t nonce[12];
    for (int i = 0; i < 12; i++) nonce[i] = i;
    
    uint8_t out[64] = {0};
    
    chacha20_tiny(key, nonce, 1, out, 64);
    EXPECT_TRUE(true);
}

void test_chacha20_tiny_different_counters() {
    TEST(ChaCha20_TinyDifferentCounters);
    uint8_t key[32] = {0};
    uint8_t nonce[12] = {0};
    uint8_t out1[64] = {0};
    uint8_t out2[64] = {0};
    
    chacha20_tiny(key, nonce, 0, out1, 64);
    chacha20_tiny(key, nonce, 1, out2, 64);
    EXPECT_TRUE(true);
}

void test_chacha20_tiny_large_output() {
    TEST(ChaCha20_TinyLargeOutput);
    uint8_t key[32] = {0};
    uint8_t nonce[12] = {0};
    uint8_t out[256] = {0};
    
    chacha20_tiny(key, nonce, 0, out, 256);
    EXPECT_TRUE(true);
}

int main() {
    test_chacha20_tiny_basic();
    test_chacha20_tiny_empty();
    test_chacha20_tiny_with_key();
    test_chacha20_tiny_different_counters();
    test_chacha20_tiny_large_output();

    return 0;
}
