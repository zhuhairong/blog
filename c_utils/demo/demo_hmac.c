/**
 * HMAC (基于哈希的消息认证码) 演示程序
 *
 * 功能：
 * - HMAC 计算
 * - 多种哈希算法支持
 * - 消息认证验证
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../c_utils/hmac.h"

// 打印十六进制
static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 演示 1: HMAC-SHA1 基本使用
static void demo_hmac_sha1(void) {
    printf("\n=== 演示 1: HMAC-SHA1 ===\n");

    hmac_error_t error;

    const uint8_t key[] = "secret_key";
    const uint8_t message[] = "Hello, World!";
    uint8_t digest[20];

    printf("密钥: %s\n", key);
    printf("消息: %s\n", message);

    if (hmac_sha1(key, sizeof(key) - 1, message, sizeof(message) - 1, digest, &error)) {
        print_hex("HMAC-SHA1", digest, 20);
        printf("\nHMAC-SHA1 输出长度: 20 字节 (160 位)\n");
    } else {
        printf("计算失败\n");
    }
}

// 演示 2: 多种算法对比
static void demo_algorithms(void) {
    printf("\n=== 演示 2: 多种 HMAC 算法 ===\n");

    hmac_error_t error;

    const uint8_t key[] = "my_secret_key";
    const uint8_t message[] = "The quick brown fox jumps over the lazy dog";

    printf("密钥: %s\n", key);
    printf("消息: %s\n\n", message);

    // HMAC-SHA1
    uint8_t sha1_digest[20];
    if (hmac_compute(HMAC_ALGO_SHA1, key, sizeof(key) - 1,
                     message, sizeof(message) - 1, sha1_digest, 20, &error)) {
        print_hex("HMAC-SHA1   (160位)", sha1_digest, 20);
    }

    // HMAC-SHA256
    uint8_t sha256_digest[32];
    if (hmac_compute(HMAC_ALGO_SHA256, key, sizeof(key) - 1,
                     message, sizeof(message) - 1, sha256_digest, 32, &error)) {
        print_hex("HMAC-SHA256 (256位)", sha256_digest, 32);
    }

    // HMAC-SHA512
    uint8_t sha512_digest[64];
    if (hmac_compute(HMAC_ALGO_SHA512, key, sizeof(key) - 1,
                     message, sizeof(message) - 1, sha512_digest, 64, &error)) {
        print_hex("HMAC-SHA512 (512位)", sha512_digest, 64);
    }

    // HMAC-MD5
    uint8_t md5_digest[16];
    if (hmac_compute(HMAC_ALGO_MD5, key, sizeof(key) - 1,
                     message, sizeof(message) - 1, md5_digest, 16, &error)) {
        print_hex("HMAC-MD5    (128位)", md5_digest, 16);
    }
}

// 演示 3: 分块计算
static void demo_incremental(void) {
    printf("\n=== 演示 3: 分块计算 ===\n");

    hmac_error_t error;
    hmac_ctx_t ctx;
    hmac_config_t config = hmac_default_config(HMAC_ALGO_SHA256);

    const uint8_t key[] = "secret";
    const uint8_t part1[] = "Hello, ";
    const uint8_t part2[] = "World";
    const uint8_t part3[] = "!";

    printf("分块消息:\n");
    printf("  块 1: %s\n", part1);
    printf("  块 2: %s\n", part2);
    printf("  块 3: %s\n", part3);

    // 初始化
    if (!hmac_init(&ctx, key, sizeof(key) - 1, &config, &error)) {
        printf("初始化失败\n");
        return;
    }

    // 分块更新
    hmac_update(&ctx, part1, sizeof(part1) - 1, &error);
    hmac_update(&ctx, part2, sizeof(part2) - 1, &error);
    hmac_update(&ctx, part3, sizeof(part3) - 1, &error);

    // 获取结果
    uint8_t digest[32];
    if (hmac_final(&ctx, digest, 32, &error)) {
        print_hex("\n分块计算 HMAC-SHA256", digest, 32);
    }

    // 对比一次性计算
    uint8_t digest2[32];
    const uint8_t full_message[] = "Hello, World!";
    hmac_compute(HMAC_ALGO_SHA256, key, sizeof(key) - 1,
                 full_message, sizeof(full_message) - 1, digest2, 32, &error);
    print_hex("一次性计算 HMAC-SHA256", digest2, 32);

    // 验证相同
    if (memcmp(digest, digest2, 32) == 0) {
        printf("\n✓ 分块计算结果与一次性计算相同\n");
    }
}

// 演示 4: 消息认证
static void demo_authentication(void) {
    printf("\n=== 演示 4: 消息认证验证 ===\n");

    hmac_error_t error;

    const uint8_t key[] = "shared_secret";
    const uint8_t message[] = "Important message";
    uint8_t mac[32];

    // 发送方计算 MAC
    printf("发送方:\n");
    printf("  消息: %s\n", message);
    hmac_compute(HMAC_ALGO_SHA256, key, sizeof(key) - 1,
                 message, sizeof(message) - 1, mac, 32, &error);
    print_hex("  计算 MAC", mac, 32);
    printf("  发送: 消息 + MAC\n");

    // 接收方验证
    printf("\n接收方:\n");
    uint8_t received_mac[32];
    memcpy(received_mac, mac, 32);

    uint8_t computed_mac[32];
    hmac_compute(HMAC_ALGO_SHA256, key, sizeof(key) - 1,
                 message, sizeof(message) - 1, computed_mac, 32, &error);

    if (memcmp(received_mac, computed_mac, 32) == 0) {
        printf("  ✓ MAC 验证通过 - 消息完整且来源可信\n");
    } else {
        printf("  ✗ MAC 验证失败 - 消息可能被篡改\n");
    }

    // 模拟篡改
    printf("\n模拟消息篡改:\n");
    const uint8_t tampered_message[] = "Tampered message";
    hmac_compute(HMAC_ALGO_SHA256, key, sizeof(key) - 1,
                 tampered_message, sizeof(tampered_message) - 1, computed_mac, 32, &error);

    if (memcmp(received_mac, computed_mac, 32) == 0) {
        printf("  ✓ MAC 验证通过\n");
    } else {
        printf("  ✗ MAC 验证失败 - 检测到篡改!\n");
    }
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. API 请求签名\n");
    printf("   - 验证请求来源\n");
    printf("   - 防止请求篡改\n");
    printf("   - 常用: HMAC-SHA256\n");
    printf("\n");

    printf("2. 数据完整性校验\n");
    printf("   - 文件传输验证\n");
    printf("   - 消息完整性\n");
    printf("\n");

    printf("3. 身份验证\n");
    printf("   - 挑战-响应协议\n");
    printf("   - 一次性密码 (HOTP/TOTP)\n");
    printf("\n");

    printf("4. JWT (JSON Web Token)\n");
    printf("   - 签名验证\n");
    printf("   - 确保 Token 未被修改\n");
    printf("\n");

    printf("5. SSL/TLS\n");
    printf("   - 握手消息认证\n");
    printf("   - 记录层完整性\n");
}

// 演示 6: 安全建议
static void demo_security_tips(void) {
    printf("\n=== 演示 6: 安全建议 ===\n");

    printf("密钥管理:\n");
    printf("  - 使用足够长的随机密钥\n");
    printf("  - 安全存储密钥\n");
    printf("  - 定期更换密钥\n");
    printf("\n");

    printf("算法选择:\n");
    printf("  - 优先使用 SHA-256 或更强算法\n");
    printf("  - 避免使用 MD5 和 SHA-1 (已不安全)\n");
    printf("  - 根据安全需求选择输出长度\n");
    printf("\n");

    printf("实现注意:\n");
    printf("  - 使用常量时间比较 MAC\n");
    printf("  - 防止时序攻击\n");
    printf("  - 正确处理密钥填充\n");
}

int main(void) {
    printf("========================================\n");
    printf("    HMAC 消息认证码演示\n");
    printf("========================================\n");

    demo_hmac_sha1();
    demo_algorithms();
    demo_incremental();
    demo_authentication();
    demo_applications();
    demo_security_tips();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
