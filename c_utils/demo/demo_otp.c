/**
 * OTP (一次性密码) 演示程序
 *
 * 功能：
 * - TOTP (基于时间)
 * - HOTP (基于计数器)
 * - 密钥生成和验证
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/otp.h"

// 演示 1: TOTP 基本概念
static void demo_totp_concept(void) {
    printf("\n=== 演示 1: TOTP (基于时间的一次性密码) ===\n");

    printf("TOTP 原理:\n\n");

    printf("1. 共享密钥\n");
    printf("   - 服务器和客户端共享一个密钥\n");
    printf("   - 通常通过二维码传递\n\n");

    printf("2. 时间步长\n");
    printf("   - 默认 30 秒\n");
    printf("   - 基于 Unix 时间戳\n\n");

    printf("3. 计算过程\n");
    printf("   - 当前时间 / 时间步长 = 时间因子\n");
    printf("   - HMAC(密钥, 时间因子)\n");
    printf("   - 动态截断得到 6-8 位数字\n\n");

    printf("4. 验证\n");
    printf("   - 客户端和服务器独立计算\n");
    printf("   - 比较结果是否匹配\n");
    printf("   - 允许少量时间偏差\n");
}

// 演示 2: HOTP 基本概念
static void demo_hotp_concept(void) {
    printf("\n=== 演示 2: HOTP (基于计数器的一次性密码) ===\n");

    printf("HOTP 原理:\n\n");

    printf("1. 共享密钥\n");
    printf("   - 与 TOTP 相同\n\n");

    printf("2. 计数器\n");
    printf("   - 单调递增的计数器\n");
    printf("   - 每次验证后增加\n\n");

    printf("3. 计算过程\n");
    printf("   - HMAC(密钥, 计数器)\n");
    printf("   - 动态截断得到验证码\n\n");

    printf("4. 同步问题\n");
    printf("   - 需要保持计数器同步\n");
    printf("   - 允许一定的窗口偏差\n");
    printf("   - 比 TOTP 更复杂\n");
}

// 演示 3: 算法选择
static void demo_algorithms(void) {
    printf("\n=== 演示 3: 哈希算法选择 ===\n");

    printf("支持的算法:\n\n");

    printf("SHA-1:\n");
    printf("  - 最广泛支持\n");
    printf("  - Google Authenticator 默认\n");
    printf("  - 安全性足够用于 2FA\n\n");

    printf("SHA-256:\n");
    printf("  - 更高的安全性\n");
    printf("  - 现代应用推荐\n");
    printf("  - 计算开销稍大\n\n");

    printf("SHA-512:\n");
    printf("  - 最高安全性\n");
    printf("  - 适用于高安全场景\n");
    printf("  - 部分客户端不支持\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    printf("TOTP 配置:\n");
    otp_totp_config_t totp_config = otp_default_totp_config();
    printf("  算法: SHA-1\n");
    printf("  位数: %d\n", totp_config.digits);
    printf("  时间步长: %d 秒\n", totp_config.time_step);
    printf("  起始时间: Unix 纪元\n\n");

    printf("HOTP 配置:\n");
    otp_hotp_config_t hotp_config = otp_default_hotp_config();
    printf("  算法: SHA-1\n");
    printf("  位数: %d\n", hotp_config.digits);
}

// 演示 5: 使用示例
static void demo_usage(void) {
    printf("\n=== 演示 5: 使用流程 ===\n");

    printf("1. 生成密钥:\n");
    printf("   - 随机生成 20 字节密钥\n");
    printf("   - Base32 编码用于二维码\n\n");

    printf("2. 用户注册:\n");
    printf("   - 显示二维码\n");
    printf("   - 用户用 Authenticator 扫描\n");
    printf("   - 验证首次代码\n\n");

    printf("3. 登录验证:\n");
    printf("   - 用户输入用户名密码\n");
    printf("   - 系统要求 OTP 代码\n");
    printf("   - 用户输入 6 位代码\n");
    printf("   - 系统验证代码有效性\n\n");

    printf("4. 备份码:\n");
    printf("   - 生成一次性备份码\n");
    printf("   - 用于设备丢失场景\n");
}

// 演示 6: 安全考虑
static void demo_security(void) {
    printf("\n=== 演示 6: 安全考虑 ===\n");

    printf("密钥安全:\n");
    printf("  - 服务器端加密存储\n");
    printf("  - 传输使用 HTTPS\n");
    printf("  - 防止密钥泄露\n\n");

    printf("时间同步:\n");
    printf("  - 服务器使用 NTP 同步\n");
    printf("  - 允许 ±1 个时间窗口\n");
    printf("  - 防止重放攻击\n\n");

    printf("速率限制:\n");
    printf("  - 限制验证尝试次数\n");
    printf("  - 防止暴力破解\n");
    printf("  - 账户锁定机制\n\n");

    printf("备份方案:\n");
    printf("  - 备份码机制\n");
    printf("  - 备用验证方式\n");
    printf("  - 客服恢复流程\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 双因素认证 (2FA)\n");
    printf("   - 网站登录\n");
    printf("   - 银行账户\n");
    printf("   - 企业系统\n\n");

    printf("2. 常用工具\n");
    printf("   - Google Authenticator\n");
    printf("   - Microsoft Authenticator\n");
    printf("   - Authy\n");
    printf("   - 1Password\n\n");

    printf("3. 实现标准\n");
    printf("   - RFC 4226 (HOTP)\n");
    printf("   - RFC 6238 (TOTP)\n\n");

    printf("4. 优势\n");
    printf("   - 无需网络连接\n");
    printf("   - 开源标准\n");
    printf("   - 广泛支持\n");
}

int main(void) {
    printf("========================================\n");
    printf("    OTP (一次性密码) 演示\n");
    printf("========================================\n");

    demo_totp_concept();
    demo_hotp_concept();
    demo_algorithms();
    demo_config();
    demo_usage();
    demo_security();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
