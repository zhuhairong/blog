/**
 * PBKDF2 密钥派生演示程序
 *
 * 功能：
 * - 密码哈希
 * - 密钥派生
 * - 算法选择
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/pbkdf2.h"

// 打印十六进制
static void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: PBKDF2 基本概念 ===\n");

    printf("PBKDF2 (Password-Based Key Derivation Function 2):\n\n");

    printf("目的:\n");
    printf("  - 从密码派生加密密钥\n");
    printf("  - 增加破解难度\n");
    printf("  - 抵御暴力破解\n\n");

    printf("原理:\n");
    printf("  1. 使用盐值 (salt) 防止彩虹表\n");
    printf("  2. 多次迭代增加计算时间\n");
    printf("  3. 使用 HMAC 作为伪随机函数\n\n");

    printf("公式:\n");
    printf("  DK = PBKDF2(PRF, Password, Salt, c, dkLen)\n");
    printf("  - PRF: 伪随机函数 (如 HMAC-SHA256)\n");
    printf("  - c: 迭代次数\n");
    printf("  - dkLen: 派生密钥长度\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    const char *password = "mysecretpassword";
    const uint8_t salt[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    size_t salt_len = sizeof(salt);
    int iterations = 10000;

    uint8_t key[32];

    printf("密码: \"%s\"\n", password);
    printf("盐值: ");
    print_hex(salt, salt_len);
    printf("\n");
    printf("迭代次数: %d\n", iterations);

    pbkdf2_sha1(password, salt, salt_len, iterations, key, sizeof(key));

    printf("派生密钥: ");
    print_hex(key, sizeof(key));
    printf("\n");
}

// 演示 3: 算法选择
static void demo_algorithms(void) {
    printf("\n=== 演示 3: 哈希算法选择 ===\n");

    printf("支持的算法:\n\n");

    printf("SHA-1:\n");
    printf("  - 传统算法\n");
    printf("  - 160 位输出\n");
    printf("  - 兼容性最好\n\n");

    printf("SHA-256:\n");
    printf("  - 推荐算法\n");
    printf("  - 256 位输出\n");
    printf("  - 安全性高\n\n");

    printf("SHA-512:\n");
    printf("  - 最高安全\n");
    printf("  - 512 位输出\n");
    printf("  - 64 位平台优化\n");
}

// 演示 4: 迭代次数影响
static void demo_iterations(void) {
    printf("\n=== 演示 4: 迭代次数影响 ===\n");

    printf("迭代次数与安全性的关系:\n\n");

    printf("迭代次数    安全性    性能影响\n");
    printf("----------  --------  ----------\n");
    printf("1,000       低        快\n");
    printf("10,000      中        中等\n");
    printf("100,000     高        慢\n");
    printf("1,000,000   很高      很慢\n\n");

    printf("推荐值:\n");
    printf("  - 一般应用: 10,000 - 100,000\n");
    printf("  - 高安全: 100,000+\n");
    printf("  - 根据硬件性能调整\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("PBKDF2 配置结构体:\n\n");

    printf("algorithm - 哈希算法:\n");
    printf("  PBKDF2_ALGORITHM_SHA1\n");
    printf("  PBKDF2_ALGORITHM_SHA256\n");
    printf("  PBKDF2_ALGORITHM_SHA512\n\n");

    printf("iterations - 迭代次数:\n");
    printf("  默认: 10000\n");
    printf("  最小: 1000\n\n");

    printf("salt_len - 盐长度:\n");
    printf("  推荐: 16-32 字节\n\n");

    printf("output_len - 输出长度:\n");
    printf("  SHA-1: 20 字节\n");
    printf("  SHA-256: 32 字节\n");
    printf("  SHA-512: 64 字节\n");
}

// 演示 6: 密码存储最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 6: 密码存储最佳实践 ===\n");

    printf("1. 使用随机盐值\n");
    printf("   - 每个密码使用不同盐值\n");
    printf("   - 盐值至少 16 字节\n");
    printf("   - 盐值随哈希一起存储\n\n");

    printf("2. 足够大的迭代次数\n");
    printf("   - 至少 10,000 次\n");
    printf("   - 根据硬件调整\n");
    printf("   - 定期增加\n\n");

    printf("3. 使用强哈希算法\n");
    printf("   - 推荐 SHA-256 或 SHA-512\n");
    printf("   - 避免单独使用 SHA-1\n\n");

    printf("4. 存储格式\n");
    printf("   - 算法:迭代次数:盐值:哈希\n");
    printf("   - 示例: sha256:10000:abcd...:1234...\n\n");

    printf("5. 考虑替代方案\n");
    printf("   - bcrypt (推荐)\n");
    printf("   - scrypt\n");
    printf("   - Argon2 (最先进)\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 密码哈希存储\n");
    printf("   - 用户密码存储\n");
    printf("   - 抵御彩虹表攻击\n");
    printf("   - 增加破解成本\n\n");

    printf("2. 加密密钥派生\n");
    printf("   - 从密码生成加密密钥\n");
    printf("   - 磁盘加密\n");
    printf("   - 文件加密\n\n");

    printf("3. WiFi 安全 (WPA2)\n");
    printf("   - 从密码派生 PMK\n");
    printf("   - 标准应用\n\n");

    printf("4. 标准实现\n");
    printf("   - RFC 2898\n");
    printf("   - PKCS #5 v2.0\n");
    printf("   - 广泛支持\n");
}

int main(void) {
    printf("========================================\n");
    printf("    PBKDF2 密钥派生演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_algorithms();
    demo_iterations();
    demo_config();
    demo_best_practices();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
