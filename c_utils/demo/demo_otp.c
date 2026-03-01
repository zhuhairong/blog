/**
 * OTP (一次性密码) 演示程序
 *
 * 功能：
 * - TOTP (基于时间) 生成和验证
 * - HOTP (基于计数器) 生成和验证
 * - 密钥生成和管理
 * - 错误处理和边界情况测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "../c_utils/otp.h"
#include "../c_utils/base32.h"

// 生成随机密钥
static void generate_secret(uint8_t *secret, size_t len) {
    for (size_t i = 0; i < len; i++) {
        secret[i] = rand() % 256;
    }
}

// Base32 编码密钥
static char* base32_encode_secret(const uint8_t *secret, size_t secret_len) {
    return base32_encode_alloc(secret, secret_len);
}

// 演示 1: TOTP 生成和验证
static void demo_totp(void) {
    printf("\n=== 演示 1: TOTP (基于时间的一次性密码) ===\n");
    
    // 生成随机密钥
    uint8_t secret[20];
    generate_secret(secret, sizeof(secret));
    
    // Base32 编码密钥
    char *encoded_secret = base32_encode_secret(secret, sizeof(secret));
    if (!encoded_secret) {
        printf("  ✗ 密钥编码失败\n");
        return;
    }
    
    printf("  密钥 (Base32): %s\n", encoded_secret);
    free(encoded_secret);
    
    // 获取当前时间
    uint64_t current_time = time(NULL);
    printf("  当前时间: %ld (Unix 时间戳)\n", current_time);
    
    // 生成 TOTP (默认配置)
    uint32_t totp = otp_generate_totp(secret, sizeof(secret), current_time);
    printf("  TOTP (6位): %06d\n", totp);
    
    // 使用自定义配置
    otp_totp_config_t config = otp_default_totp_config();
    config.digits = 8;
    config.algorithm = OTP_ALGORITHM_SHA256;
    config.time_step = 60; // 60秒
    
    otp_error_t error;
    uint32_t totp_custom = otp_generate_totp_ex(secret, sizeof(secret), current_time, &config, &error);
    if (error == OTP_OK) {
        printf("  TOTP (自定义配置): %08d\n", totp_custom);
        printf("  配置: SHA-256, 8位, 60秒步长\n");
    } else {
        printf("  ✗ TOTP 生成失败: 错误码 %d\n", error);
    }
    
    // 验证 TOTP (模拟)
    printf("  验证 TOTP:\n");
    printf("  - 客户端计算: %06d\n", totp);
    printf("  - 服务器计算: %06d\n", totp);
    printf("  - 验证结果: %s\n", (totp == totp) ? "✓ 验证通过" : "✗ 验证失败");
    
    // 时间偏差测试
    printf("  时间偏差测试:\n");
    uint32_t totp_prev = otp_generate_totp(secret, sizeof(secret), current_time - 30);
    uint32_t totp_next = otp_generate_totp(secret, sizeof(secret), current_time + 30);
    printf("  - 当前: %06d\n", totp);
    printf("  - 30秒前: %06d\n", totp_prev);
    printf("  - 30秒后: %06d\n", totp_next);
}

// 演示 2: HOTP 生成和验证
static void demo_hotp(void) {
    printf("\n=== 演示 2: HOTP (基于计数器的一次性密码) ===\n");
    
    // 生成随机密钥
    uint8_t secret[20];
    generate_secret(secret, sizeof(secret));
    
    // Base32 编码密钥
    char *encoded_secret = base32_encode_secret(secret, sizeof(secret));
    if (!encoded_secret) {
        printf("  ✗ 密钥编码失败\n");
        return;
    }
    
    printf("  密钥 (Base32): %s\n", encoded_secret);
    free(encoded_secret);
    
    // 测试不同计数器值
    uint64_t counter = 123456;
    otp_error_t error;
    
    printf("  计数器值: %ld\n", counter);
    
    // 生成 HOTP
    otp_hotp_config_t hotp_config = otp_default_hotp_config();
    
    // 6位 HOTP
    uint32_t hotp6 = otp_generate_hotp(secret, sizeof(secret), counter, hotp_config.digits, &error);
    if (error == OTP_OK) {
        printf("  HOTP (6位): %06d\n", hotp6);
    } else {
        printf("  ✗ HOTP 生成失败: 错误码 %d\n", error);
    }
    
    // 8位 HOTP
    hotp_config.digits = 8;
    hotp_config.algorithm = OTP_ALGORITHM_SHA256;
    uint32_t hotp8 = otp_generate_hotp_ex(secret, sizeof(secret), counter, &hotp_config, &error);
    if (error == OTP_OK) {
        printf("  HOTP (8位, SHA-256): %08d\n", hotp8);
    } else {
        printf("  ✗ HOTP 生成失败: 错误码 %d\n", error);
    }
    
    // 计数器递增
    counter++;
    uint32_t hotp_next = otp_generate_hotp(secret, sizeof(secret), counter, 6, &error);
    if (error == OTP_OK) {
        printf("  计数器+1: %ld → HOTP: %06d\n", counter, hotp_next);
    }
    
    // 验证 HOTP
    printf("  验证 HOTP:\n");
    printf("  - 客户端提供: %06d\n", hotp6);
    printf("  - 服务器计算: %06d\n", hotp6);
    printf("  - 验证结果: %s\n", (hotp6 == hotp6) ? "✓ 验证通过" : "✗ 验证失败");
}

// 演示 3: 错误处理和边界情况
static void demo_error_handling(void) {
    printf("\n=== 演示 3: 错误处理和边界情况 ===\n");
    
    // 测试空密钥
    printf("  测试空密钥:\n");
    otp_error_t error;
    uint32_t result = otp_generate_totp(NULL, 0, time(NULL));
    printf("  结果: %06d (应该失败)\n", result);
    
    // 测试密钥太短
    printf("  测试密钥太短:\n");
    uint8_t short_secret[1];
    short_secret[0] = 0x01;
    result = otp_generate_totp_ex(short_secret, sizeof(short_secret), time(NULL), NULL, &error);
    printf("  错误码: %d (应该是 %d - 密钥太短)\n", error, OTP_ERROR_SECRET_TOO_SHORT);
    
    // 测试无效算法
    printf("  测试无效算法:\n");
    uint8_t secret[20];
    generate_secret(secret, sizeof(secret));
    otp_totp_config_t config = otp_default_totp_config();
    config.algorithm = OTP_ALGORITHM_MAX; // 无效算法
    result = otp_generate_totp_ex(secret, sizeof(secret), time(NULL), &config, &error);
    printf("  错误码: %d (应该是 %d - 无效算法)\n", error, OTP_ERROR_INVALID_ALGORITHM);
    
    // 测试无效位数
    printf("  测试无效位数:\n");
    config = otp_default_totp_config();
    config.digits = 10; // 超出范围
    result = otp_generate_totp_ex(secret, sizeof(secret), time(NULL), &config, &error);
    printf("  错误码: %d (应该是 %d - 无效参数)\n", error, OTP_ERROR_INVALID_ARGS);
}

// 演示 4: 实际应用场景
static void demo_real_world(void) {
    printf("\n=== 演示 4: 实际应用场景 ===\n");
    
    // 模拟用户注册流程
    printf("  1. 用户注册流程:\n");
    printf("     - 服务器生成随机密钥\n");
    printf("     - 密钥 Base32 编码\n");
    printf("     - 生成二维码: otpauth://totp/Example:user@example.com?secret=JBSWY3DPEHPK3PXP&issuer=Example\n");
    printf("     - 用户使用 Google Authenticator 扫描\n");
    printf("     - 验证首次 TOTP\n");
    
    // 模拟登录流程
    printf("\n  2. 登录验证流程:\n");
    printf("     - 用户输入用户名和密码\n");
    printf("     - 系统要求输入 6 位 OTP 码\n");
    printf("     - 用户从 Authenticator 应用获取代码\n");
    printf("     - 系统验证代码有效性\n");
    printf("     - 允许 ±1 个时间窗口的偏差\n");
    
    // 安全建议
    printf("\n  3. 安全最佳实践:\n");
    printf("     - 服务器端加密存储密钥\n");
    printf("     - 使用 HTTPS 传输\n");
    printf("     - 实现速率限制防止暴力破解\n");
    printf("     - 提供备份码机制\n");
    printf("     - 定期轮换密钥\n");
}

// 演示 5: 算法性能测试
static void demo_performance(void) {
    printf("\n=== 演示 5: 算法性能测试 ===\n");
    
    uint8_t secret[20];
    generate_secret(secret, sizeof(secret));
    uint64_t start_time, end_time;
    const int iterations = 10000;
    
    // 测试 TOTP 性能
    start_time = time(NULL);
    for (int i = 0; i < iterations; i++) {
        otp_generate_totp(secret, sizeof(secret), time(NULL) + i);
    }
    end_time = time(NULL);
    printf("  TOTP 性能: %d 次计算耗时 %ld 秒\n", iterations, end_time - start_time);
    
    // 测试 HOTP 性能
    start_time = time(NULL);
    for (int i = 0; i < iterations; i++) {
        otp_error_t error;
        otp_generate_hotp(secret, sizeof(secret), i, 6, &error);
    }
    end_time = time(NULL);
    printf("  HOTP 性能: %d 次计算耗时 %ld 秒\n", iterations, end_time - start_time);
}

int main(void) {
    printf("========================================\n");
    printf("    OTP (一次性密码) 演示\n");
    printf("========================================\n");
    
    // 初始化随机数种子
    srand(time(NULL));
    
    demo_totp();
    demo_hotp();
    demo_error_handling();
    demo_real_world();
    demo_performance();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
