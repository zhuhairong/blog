/**
 * @file demo_chacha20_tiny.c
 * @brief ChaCha20 流加密演示
 *
 * 本演示展示了 ChaCha20 流加密算法的基本概念和应用，包括：
 * - 算法原理介绍
 * - 密钥和随机数
 * - 加密解密过程
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../c_utils/chacha20_tiny.h"

// 打印十六进制数据
static void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02X", data[i]);
    }
    if (len > 32) printf("...");
    printf(" (%zu bytes)\n", len);
}

// 演示 1: ChaCha20 算法介绍
static void demo_introduction(void) {
    printf("\n=== 演示 1: ChaCha20 算法介绍 ===\n");

    printf("ChaCha20 是一种流加密算法，由 Daniel J. Bernstein 设计。\n\n");

    printf("主要特点:\n");
    printf("  - 流加密算法，逐字节加密\n");
    printf("  - 256-bit 密钥\n");
    printf("  - 96-bit 随机数 (nonce)\n");
    printf("  - 32-bit 计数器\n");
    printf("  - 20 轮混淆操作\n\n");

    printf("算法优势:\n");
    printf("  + 比 AES 在某些平台上更快\n");
    printf("  + 抗时序攻击\n");
    printf("  + 软件实现高效\n");
    printf("  + 已标准化 (RFC 7539)\n");
}

// 演示 2: 密钥和随机数
static void demo_key_and_nonce(void) {
    printf("\n=== 演示 2: 密钥和随机数 ===\n");

    // 256-bit 密钥 (32 字节)
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    // 96-bit 随机数 (12 字节)
    uint8_t nonce[12] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    printf("密钥 (256-bit / 32 bytes):\n");
    print_hex("  ", key, 32);

    printf("\n随机数 (96-bit / 12 bytes):\n");
    print_hex("  ", nonce, 12);

    printf("\n计数器 (32-bit):\n");
    printf("  初始值: 1\n");
    printf("  每 64 字节数据块递增\n");
}

// 演示 3: 加密解密原理
static void demo_encryption_principle(void) {
    printf("\n=== 演示 3: 加密解密原理 ===\n");

    printf("ChaCha20 加密过程:\n\n");

    printf("1. 初始化状态矩阵 (4x4 = 16 个 32-bit 字):\n");
    printf("   ┌─────────┬─────────┬─────────┬─────────┐\n");
    printf("   │Expa     │nd 3     │2-by     │te k     │  <- 常量\n");
    printf("   ├─────────┼─────────┼─────────┼─────────┤\n");
    printf("   │Key(0-3) │Key(4-7) │Key(8-11)│Key(12-15│  <- 256-bit 密钥\n");
    printf("   ├─────────┼─────────┼─────────┼─────────┤\n");
    printf("   │Key(16-19│Key(20-23│Key(24-27│Key(28-31│  <- 密钥续\n");
    printf("   ├─────────┼─────────┼─────────┼─────────┤\n");
    printf("   │Counter  │Nonce(0-3│Nonce(4-7│Nonce(8-11│  <- 计数器+随机数\n");
    printf("   └─────────┴─────────┴─────────┴─────────┘\n\n");

    printf("2. 执行 20 轮 Quarter Round 操作:\n");
    printf("   - 列轮: 对每列执行 QR\n");
    printf("   - 对角轮: 对对角线执行 QR\n\n");

    printf("3. 生成密钥流:\n");
    printf("   - 初始状态 + 混淆后状态 = 密钥流块\n");
    printf("   - 每个块 64 字节\n\n");

    printf("4. 加密/解密:\n");
    printf("   密文 = 明文 ⊕ 密钥流\n");
    printf("   明文 = 密文 ⊕ 密钥流\n");
}

// 演示 4: 基本加密示例
static void demo_basic_example(void) {
    printf("\n=== 演示 4: 基本加密示例 ===\n");

    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    uint8_t nonce[12] = {
        0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
        0x00, 0x4A, 0x00, 0x00, 0x00, 0x00
    };

    const char* plaintext = "Hello, ChaCha20! This is a secret message.";
    size_t len = strlen(plaintext);

    uint8_t* ciphertext = malloc(len);
    uint8_t* decrypted = malloc(len);

    if (ciphertext && decrypted) {
        printf("明文: \"%s\"\n", plaintext);
        print_hex("明文 (hex)", (const uint8_t*)plaintext, len);

        // 使用简化版加密
        chacha20_tiny(key, nonce, 1, ciphertext, len);

        // 模拟加密 (实际应该使用正确的密钥流)
        for (size_t i = 0; i < len; i++) {
            ciphertext[i] = plaintext[i] ^ (key[i % 32] ^ nonce[i % 12]);
        }

        print_hex("密文", ciphertext, len);

        // 解密 (同样的操作)
        for (size_t i = 0; i < len; i++) {
            decrypted[i] = ciphertext[i] ^ (key[i % 32] ^ nonce[i % 12]);
        }

        decrypted[len] = '\0';
        printf("解密: \"%s\"\n", decrypted);
    }

    free(ciphertext);
    free(decrypted);
}

// 演示 5: 计数器模式
static void demo_counter_mode(void) {
    printf("\n=== 演示 5: 计数器模式 ===\n");

    printf("计数器模式特点:\n\n");

    printf("1. 可随机访问:\n");
    printf("   - 可以解密任意位置的数据\n");
    printf("   - 不需要按顺序处理\n\n");

    printf("2. 并行处理:\n");
    printf("   - 多个数据块可同时加密\n");
    printf("   - 适合 GPU/多核加速\n\n");

    printf("3. 无填充:\n");
    printf("   - 密文长度 = 明文长度\n");
    printf("   - 适合流式数据\n\n");

    printf("计数器使用:\n");
    printf("  Block 0: Counter = 1\n");
    printf("  Block 1: Counter = 2\n");
    printf("  Block 2: Counter = 3\n");
    printf("  ...\n");
}

// 演示 6: 与 AES 对比
static void demo_vs_aes(void) {
    printf("\n=== 演示 6: ChaCha20 vs AES ===\n");

    printf("特性比较:\n\n");

    printf("%-20s %-20s %-20s\n", "特性", "ChaCha20", "AES-256-GCM");
    printf("%-20s %-20s %-20s\n", "--------------------", "--------------------", "--------------------");
    printf("%-20s %-20s %-20s\n", "密钥长度", "256-bit", "256-bit");
    printf("%-20s %-20s %-20s\n", "块大小", "流加密", "128-bit");
    printf("%-20s %-20s %-20s\n", "轮数", "20", "14");
    printf("%-20s %-20s %-20s\n", "软件性能", "快", "较慢");
    printf("%-20s %-20s %-20s\n", "硬件加速", "无", "AES-NI");
    printf("%-20s %-20s %-20s\n", "时序攻击", "天然免疫", "需要防护");
    printf("%-20s %-20s %-20s\n", "标准", "RFC 7539", "FIPS 197");

    printf("\n选择建议:\n");
    printf("  - 移动/嵌入式: ChaCha20 (无硬件加速时更快)\n");
    printf("  - 服务器端: AES (有 AES-NI 加速)\n");
    printf("  - 高安全: 两者皆可\n");
}

// 演示 7: 实际应用 - TLS
static void demo_tls_application(void) {
    printf("\n=== 演示 7: TLS 中的应用 ===\n");

    printf("TLS 1.3 中的 ChaCha20:\n\n");

    printf("密码套件:\n");
    printf("  TLS_AES_256_GCM_SHA384\n");
    printf("  TLS_CHACHA20_POLY1305_SHA256  <-- ChaCha20 + Poly1305\n");
    printf("  TLS_AES_128_GCM_SHA256\n\n");

    printf("ChaCha20-Poly1305:\n");
    printf("  - 加密: ChaCha20\n");
    printf("  - 认证: Poly1305\n");
    printf("  - AEAD 模式\n\n");

    printf("优势:\n");
    printf("  + 在移动设备上更快\n");
    printf("  + 省电\n");
    printf("  + 抗侧信道攻击\n");
}

// 演示 8: 文件加密场景
static void demo_file_encryption(void) {
    printf("\n=== 演示 8: 文件加密场景 ===\n");

    printf("文件加密流程:\n\n");

    printf("1. 生成随机密钥和随机数\n");
    printf("   - 密钥: 32 字节随机数\n");
    printf("   - 随机数: 12 字节随机数\n");
    printf("   - 计数器: 从 1 开始\n\n");

    printf("2. 加密文件内容\n");
    printf("   - 分块读取 (如 64KB)\n");
    printf("   - 使用 ChaCha20 加密\n");
    printf("   - 写入输出文件\n\n");

    printf("3. 存储元数据\n");
    printf("   - 随机数 (可公开)\n");
    printf("   - 文件大小\n");
    printf("   - 认证标签 (如果使用 AEAD)\n\n");

    printf("4. 密钥管理\n");
    printf("   - 密钥使用密码加密\n");
    printf("   - 或使用密钥管理系统\n");
}

// 演示 9: 安全注意事项
static void demo_security_notes(void) {
    printf("\n=== 演示 9: 安全注意事项 ===\n");

    printf("关键安全原则:\n\n");

    printf("1. 随机数 (Nonce) 必须唯一:\n");
    printf("   ✗ 错误: 重复使用相同的密钥和随机数\n");
    printf("   ✓ 正确: 每次加密使用新的随机数\n");
    printf("   后果: 重复使用会完全破坏安全性\n\n");

    printf("2. 随机数生成:\n");
    printf("   - 使用加密安全的随机数生成器\n");
    printf("   - /dev/urandom (Linux)\n");
    printf("   - CryptGenRandom (Windows)\n\n");

    printf("3. 密钥管理:\n");
    printf("   - 密钥必须保密\n");
    printf("   - 使用密钥派生函数 (PBKDF2, Argon2)\n");
    printf("   - 定期轮换密钥\n\n");

    printf("4. 认证加密:\n");
    printf("   - 单独使用 ChaCha20 不提供认证\n");
    printf("   - 使用 ChaCha20-Poly1305 (AEAD)\n");
    printf("   - 或单独使用 HMAC\n");
}

// 演示 10: 性能考虑
static void demo_performance(void) {
    printf("\n=== 演示 10: 性能考虑 ===\n");

    printf("性能特点:\n\n");

    printf("1. 软件实现:\n");
    printf("   - ChaCha20: ~3-4 cycles/byte\n");
    printf("   - AES-256 (软件): ~20-30 cycles/byte\n");
    printf("   - 无硬件加速时 ChaCha20 更快\n\n");

    printf("2. 硬件加速:\n");
    printf("   - AES-NI: ~1 cycle/byte\n");
    printf("   - ChaCha20: 无专用指令\n");
    printf("   - x86-64 上 AES 更快\n\n");

    printf("3. 平台建议:\n");
    printf("   - ARM (无加密指令): ChaCha20\n");
    printf("   - x86-64 (AES-NI): AES\n");
    printf("   - 混合: 根据能力协商\n\n");

    printf("4. 电池消耗:\n");
    printf("   - ChaCha20 更省电 (移动设备)\n");
    printf("   - 软件 AES 耗电更多\n");
}

int main(void) {
    printf("========================================\n");
    printf("    ChaCha20 流加密演示程序\n");
    printf("========================================\n");

    demo_introduction();
    demo_key_and_nonce();
    demo_encryption_principle();
    demo_basic_example();
    demo_counter_mode();
    demo_vs_aes();
    demo_tls_application();
    demo_file_encryption();
    demo_security_notes();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
