/**
 * Poly1305 消息认证码演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/poly1305_tiny.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

static void demo_basic_mac(void) {
    printf("\n=== 演示 1: 基本 MAC 计算 ===\n");

    uint8_t key[32] = {
        0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33,
        0x7f, 0x44, 0x52, 0xfe, 0x42, 0xd5, 0x06, 0xa8,
        0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d, 0xb2, 0xfd,
        0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b
    };

    const char *message = "Cryptographic Message Authentication";
    size_t msg_len = strlen(message);

    uint8_t mac[16];

    printf("消息: \"%s\"\n", message);
    print_hex("密钥", key, 32);

    poly1305_tiny(key, (const uint8_t *)message, msg_len, mac);
    print_hex("\nMAC", mac, 16);
}

static void demo_different_messages(void) {
    printf("\n=== 演示 2: 不同消息产生不同 MAC ===\n");

    uint8_t key[32];
    memset(key, 0x11, 32);

    const char *messages[] = {
        "Message A",
        "Message B",
        "Message A"
    };

    uint8_t macs[3][16];

    printf("使用相同密钥:\n\n");
    for (int i = 0; i < 3; i++) {
        poly1305_tiny(key, (const uint8_t *)messages[i], strlen(messages[i]), macs[i]);
        printf("消息: \"%s\"\n", messages[i]);
        print_hex("MAC", macs[i], 16);
        printf("\n");
    }

    bool same_0_2 = memcmp(macs[0], macs[2], 16) == 0;
    bool same_0_1 = memcmp(macs[0], macs[1], 16) == 0;

    printf("消息0 和 消息2 MAC 相同: %s (预期: 是)\n", same_0_2 ? "是" : "否");
    printf("消息0 和 消息1 MAC 相同: %s (预期: 否)\n", same_0_1 ? "是" : "否");
}

static void demo_different_keys(void) {
    printf("\n=== 演示 3: 不同密钥产生不同 MAC ===\n");

    uint8_t key1[32], key2[32];
    memset(key1, 0xAA, 32);
    memset(key2, 0xBB, 32);

    const char *message = "Same message with different keys";

    uint8_t mac1[16], mac2[16];

    poly1305_tiny(key1, (const uint8_t *)message, strlen(message), mac1);
    poly1305_tiny(key2, (const uint8_t *)message, strlen(message), mac2);

    printf("消息: \"%s\"\n\n", message);
    print_hex("密钥1 MAC", mac1, 16);
    print_hex("密钥2 MAC", mac2, 16);

    bool same = memcmp(mac1, mac2, 16) == 0;
    printf("\nMAC 相同: %s (预期: 否)\n", same ? "是" : "否");
}

static void demo_empty_message(void) {
    printf("\n=== 演示 4: 空消息 MAC ===\n");

    uint8_t key[32];
    memset(key, 0x42, 32);

    uint8_t mac[16];

    poly1305_tiny(key, (const uint8_t *)"", 0, mac);

    printf("空消息 MAC:\n");
    print_hex("MAC", mac, 16);
}

int main(void) {
    printf("========================================\n");
    printf("    Poly1305 消息认证码演示\n");
    printf("========================================\n");

    demo_basic_mac();
    demo_different_messages();
    demo_different_keys();
    demo_empty_message();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
