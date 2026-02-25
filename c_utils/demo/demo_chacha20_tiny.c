/**
 * ChaCha20 流加密演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/chacha20_tiny.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02x", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

static void demo_basic_encrypt(void) {
    printf("\n=== 演示 1: 基本加密 ===\n");

    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t nonce[12] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a,
        0x00, 0x00, 0x00, 0x00
    };

    const char *plaintext = "Hello, ChaCha20!";
    size_t len = strlen(plaintext);

    uint8_t keystream[64];

    printf("原始数据: \"%s\"\n", plaintext);
    printf("密钥: ");
    for (int i = 0; i < 8; i++) printf("%02x", key[i]);
    printf("...\n");
    printf("随机数: ");
    for (int i = 0; i < 12; i++) printf("%02x", nonce[i]);
    printf("\n\n");

    chacha20_tiny(key, nonce, 1, keystream, len);

    print_hex("密钥流", keystream, len);

    printf("\n加密 (异或):\n");
    uint8_t ciphertext[64];
    for (size_t i = 0; i < len; i++) {
        ciphertext[i] = (uint8_t)plaintext[i] ^ keystream[i];
    }
    print_hex("密文", ciphertext, len);

    printf("\n解密 (再次异或):\n");
    uint8_t decrypted[64];
    for (size_t i = 0; i < len; i++) {
        decrypted[i] = ciphertext[i] ^ keystream[i];
    }
    decrypted[len] = '\0';
    printf("明文: \"%s\"\n", decrypted);
}

static void demo_key_uniqueness(void) {
    printf("\n=== 演示 2: 不同密钥产生不同密钥流 ===\n");

    uint8_t key1[32], key2[32];
    memset(key1, 0x11, 32);
    memset(key2, 0x22, 32);

    uint8_t nonce[12];
    memset(nonce, 0x00, 12);

    uint8_t stream1[64], stream2[64];

    chacha20_tiny(key1, nonce, 0, stream1, 64);
    chacha20_tiny(key2, nonce, 0, stream2, 64);

    printf("密钥1: 0x11... (32字节)\n");
    printf("密钥2: 0x22... (32字节)\n");
    printf("相同随机数\n\n");

    print_hex("密钥流1", stream1, 32);
    print_hex("密钥流2", stream2, 32);

    bool same = memcmp(stream1, stream2, 64) == 0;
    printf("\n密钥流相同: %s (预期: 否)\n", same ? "是" : "否");
}

static void demo_nonce_uniqueness(void) {
    printf("\n=== 演示 3: 不同随机数产生不同密钥流 ===\n");

    uint8_t key[32];
    memset(key, 0xAB, 32);

    uint8_t nonce1[12], nonce2[12];
    memset(nonce1, 0x00, 12);
    memset(nonce2, 0x01, 12);

    uint8_t stream1[64], stream2[64];

    chacha20_tiny(key, nonce1, 0, stream1, 64);
    chacha20_tiny(key, nonce2, 0, stream2, 64);

    printf("相同密钥\n");
    printf("随机数1: 0x00... (12字节)\n");
    printf("随机数2: 0x01... (12字节)\n\n");

    print_hex("密钥流1", stream1, 32);
    print_hex("密钥流2", stream2, 32);

    bool same = memcmp(stream1, stream2, 64) == 0;
    printf("\n密钥流相同: %s (预期: 否)\n", same ? "是" : "否");
}

static void demo_counter_effect(void) {
    printf("\n=== 演示 4: 计数器影响 ===\n");

    uint8_t key[32];
    memset(key, 0xCD, 32);

    uint8_t nonce[12];
    memset(nonce, 0x00, 12);

    uint8_t stream1[64], stream2[64];

    chacha20_tiny(key, nonce, 0, stream1, 64);
    chacha20_tiny(key, nonce, 1, stream2, 64);

    printf("相同密钥和随机数\n");
    printf("计数器1: 0\n");
    printf("计数器2: 1\n\n");

    print_hex("密钥流1", stream1, 32);
    print_hex("密钥流2", stream2, 32);

    bool same = memcmp(stream1, stream2, 64) == 0;
    printf("\n密钥流相同: %s (预期: 否)\n", same ? "是" : "否");
}

int main(void) {
    printf("========================================\n");
    printf("    ChaCha20 流加密演示\n");
    printf("========================================\n");

    demo_basic_encrypt();
    demo_key_uniqueness();
    demo_nonce_uniqueness();
    demo_counter_effect();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
