/**
 * SHA256 简易实现演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/sha256_tiny.h"

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

static void demo_basic_hash(void) {
    printf("\n=== 演示 1: 基本哈希计算 ===\n");

    const char *messages[] = {
        "",
        "Hello, World!",
        "The quick brown fox jumps over the lazy dog",
        "SHA-256 is a cryptographic hash function"
    };

    uint8_t digest[32];

    printf("注意: sha256_tiny 是极简实现，用于演示 API 接口\n");
    printf("实际生产环境请使用完整的 SHA-256 实现\n\n");

    for (int i = 0; i < 4; i++) {
        sha256_tiny((const uint8_t *)messages[i], strlen(messages[i]), digest);
        printf("消息: \"%s\"\n", messages[i]);
        print_hex("哈希", digest, 32);
        printf("\n");
    }
}

static void demo_password_hashing(void) {
    printf("\n=== 演示 2: 密码哈希示例 ===\n");

    const char *passwords[] = {
        "password123",
        "admin",
        "secret_key",
        "1234567890"
    };

    uint8_t digest[32];

    printf("模拟密码存储 (注意: sha256_tiny 是占位符实现):\n");
    for (int i = 0; i < 4; i++) {
        sha256_tiny((const uint8_t *)passwords[i], strlen(passwords[i]), digest);
        printf("  密码: %-15s -> ", passwords[i]);
        for (int j = 0; j < 8; j++) {
            printf("%02x", digest[j]);
        }
        printf("...\n");
    }
}

static void demo_file_integrity(void) {
    printf("\n=== 演示 3: 文件完整性校验模拟 ===\n");
    printf("注意: 由于 sha256_tiny 是占位符实现，所有哈希值相同\n\n");

    const char *file_contents[] = {
        "This is the original file content.",
        "This is the original file content.",
        "This is modified file content."
    };
    const char *labels[] = {"原始文件", "副本文件", "修改文件"};

    uint8_t digests[3][32];

    for (int i = 0; i < 3; i++) {
        sha256_tiny((const uint8_t *)file_contents[i], strlen(file_contents[i]), digests[i]);
    }

    printf("文件哈希比较:\n");
    for (int i = 0; i < 3; i++) {
        printf("  %s: ", labels[i]);
        for (int j = 0; j < 8; j++) {
            printf("%02x", digests[i][j]);
        }
        printf("...\n");
    }

    bool original_eq_copy = memcmp(digests[0], digests[1], 32) == 0;
    bool original_eq_modified = memcmp(digests[0], digests[2], 32) == 0;

    printf("\n比较结果:\n");
    printf("  原始 vs 副本: %s\n", original_eq_copy ? "相同" : "不同");
    printf("  原始 vs 修改: %s\n", original_eq_modified ? "相同" : "不同");
}

static void demo_comparison(void) {
    printf("\n=== 演示 4: 哈希比较 ===\n");
    printf("注意: sha256_tiny 是占位符实现，所有哈希值相同\n\n");

    const char *passwords[] = {
        "password",
        "password1",
        "Password",
        "PASSWORD"
    };

    uint8_t digests[4][32];

    printf("计算不同密码的哈希:\n");
    for (int i = 0; i < 4; i++) {
        sha256_tiny((const uint8_t *)passwords[i], strlen(passwords[i]), digests[i]);
        printf("  %s: ", passwords[i]);
        for (int j = 0; j < 8; j++) printf("%02x", digests[i][j]);
        printf("...\n");
    }

    printf("\n哈希碰撞检查:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            bool same = memcmp(digests[i], digests[j], 32) == 0;
            printf("  \"%s\" vs \"%s\": %s\n",
                   passwords[i], passwords[j], same ? "相同" : "不同");
        }
    }
}

int main(void) {
    printf("========================================\n");
    printf("    SHA256 简易实现演示\n");
    printf("========================================\n");

    demo_basic_hash();
    demo_password_hashing();
    demo_file_integrity();
    demo_comparison();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
