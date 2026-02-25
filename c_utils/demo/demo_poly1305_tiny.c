/**
 * Poly1305 消息认证码演示程序
 *
 * 功能：
 * - MAC 计算
 * - 增量计算
 * - 验证
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/poly1305_tiny.h"

// 打印十六进制
static void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: Poly1305 基本概念 ===\n");

    printf("Poly1305:\n\n");

    printf("特点:\n");
    printf("  - 高速消息认证码\n");
    printf("  - 由 Daniel J. Bernstein 设计\n");
    printf("  - 与 AES/ChaCha20 配合使用\n\n");

    printf("工作原理:\n");
    printf("  - 使用 256 位密钥\n");
    printf("  - 生成 128 位 MAC\n");
    printf("  - 基于多项式求值\n\n");

    printf("安全性:\n");
    printf("  - 密钥只能使用一次\n");
    printf("  - 提供消息完整性和认证\n");
    printf("  - 抗碰撞攻击\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    // 32 字节密钥 (256 位)
    uint8_t key[32] = {
        0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33,
        0x7f, 0x44, 0x52, 0xfe, 0x42, 0xd5, 0x06, 0xa8,
        0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d, 0xb2, 0xfd,
        0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b
    };

    const char *message = "Hello, Poly1305!";
    uint8_t mac[16];

    printf("密钥: ");
    print_hex(key, 16);
    printf("...\n");
    printf("消息: \"%s\"\n", message);

    poly1305_tiny(key, (const uint8_t*)message, strlen(message), mac);

    printf("MAC:  ");
    print_hex(mac, 16);
    printf("\n");
}

// 演示 3: 增量计算概念
static void demo_incremental(void) {
    printf("\n=== 演示 3: 增量计算 ===\n");

    printf("增量计算 API:\n\n");

    printf("初始化:\n");
    printf("  poly1305_init(&ctx, key, key_len, &error)\n");
    printf("  - 初始化上下文\n");
    printf("  - 设置密钥\n\n");

    printf("更新:\n");
    printf("  poly1305_update(&ctx, data, len, &error)\n");
    printf("  - 添加数据块\n");
    printf("  - 可多次调用\n\n");

    printf("完成:\n");
    printf("  poly1305_final(&ctx, mac, mac_len, &error)\n");
    printf("  - 计算最终 MAC\n");
    printf("  - 清理上下文\n\n");

    printf("使用场景:\n");
    printf("  - 流式数据\n");
    printf("  - 大文件处理\n");
    printf("  - 网络数据包\n");
}

// 演示 4: 验证
static void demo_verify(void) {
    printf("\n=== 演示 4: MAC 验证 ===\n");

    uint8_t key[32];
    for (int i = 0; i < 32; i++) key[i] = 0x90 + i;

    const char *message = "Important message";
    uint8_t mac[16];

    // 计算 MAC
    poly1305_tiny(key, (const uint8_t*)message, strlen(message), mac);

    printf("原始消息: \"%s\"\n", message);
    printf("MAC: ");
    print_hex(mac, 16);
    printf("\n\n");

    // 验证正确消息
    printf("验证正确消息...\n");
    uint8_t verify_mac[16];
    poly1305_tiny(key, (const uint8_t*)message, strlen(message), verify_mac);

    bool match = true;
    for (int i = 0; i < 16; i++) {
        if (mac[i] != verify_mac[i]) {
            match = false;
            break;
        }
    }
    printf("  结果: %s\n", match ? "验证通过" : "验证失败");

    // 验证篡改消息
    printf("\n验证篡改消息...\n");
    const char *tampered = "Important message!";
    poly1305_tiny(key, (const uint8_t*)tampered, strlen(tampered), verify_mac);

    match = true;
    for (int i = 0; i < 16; i++) {
        if (mac[i] != verify_mac[i]) {
            match = false;
            break;
        }
    }
    printf("  结果: %s (预期: 验证失败)\n", match ? "验证通过" : "验证失败");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("Poly1305 常量:\n");
    printf("  POLY1305_KEY_SIZE = %d 字节\n", POLY1305_KEY_SIZE);
    printf("  POLY1305_MAC_SIZE = %d 字节\n", POLY1305_MAC_SIZE);

    printf("\n配置结构体:\n");
    printf("  key_size - 密钥大小\n");
    printf("  mac_size - MAC 大小\n");
    printf("  verify_only - 仅验证模式\n");
}

// 演示 6: ChaCha20-Poly1305
static void demo_chacha20_poly1305(void) {
    printf("\n=== 演示 6: ChaCha20-Poly1305 AEAD ===\n");

    printf("ChaCha20-Poly1305:\n\n");

    printf("组成:\n");
    printf("  - ChaCha20: 流加密算法\n");
    printf("  - Poly1305: 消息认证\n\n");

    printf("工作流程:\n");
    printf("  1. 使用 ChaCha20 加密数据\n");
    printf("  2. 从 ChaCha20 密钥流派生 Poly1305 密钥\n");
    printf("  3. 计算密文的 Poly1305 MAC\n");
    printf("  4. 发送: 密文 + MAC\n\n");

    printf("优势:\n");
    printf("  - 高速 (软件实现)\n");
    printf("  - 安全 (无已知攻击)\n");
    printf("  - 标准化 (RFC 8439)\n");
    printf("  - 广泛支持 (TLS 1.3)\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. TLS 1.3\n");
    printf("   - ChaCha20-Poly1305 密码套件\n");
    printf("   - 移动设备优先\n");
    printf("   - 无硬件 AES 时的选择\n\n");

    printf("2. 磁盘加密\n");
    printf("   - 数据完整性保护\n");
    printf("   - 防篡改检测\n\n");

    printf("3. 消息认证\n");
    printf("   - 确保消息完整性\n");
    printf("   - 验证发送者身份\n\n");

    printf("4. 文件校验\n");
    printf("   - 文件完整性验证\n");
    printf("   - 下载校验\n\n");

    printf("5. 网络协议\n");
    printf("   - WireGuard VPN\n");
    printf("   - QUIC 协议\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Poly1305 消息认证码演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_incremental();
    demo_verify();
    demo_config();
    demo_chacha20_poly1305();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
