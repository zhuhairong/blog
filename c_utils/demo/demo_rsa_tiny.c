/**
 * RSA 精简版演示程序
 *
 * 功能：
 * - 模幂运算
 * - 基础 RSA 操作
 * - 适合学习和简单应用
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../c_utils/rsa_tiny.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: RSA 精简版基本概念 ===\n");

    printf("RSA 精简版:\n\n");

    printf("特点:\n");
    printf("  - 使用 64 位整数\n");
    printf("  - 适合学习和演示\n");
    printf("  - 不适合生产环境\n\n");

    printf("核心运算:\n");
    printf("  - 模幂运算: (base^exp) %% mod\n");
    printf("  - 快速幂算法\n");
    printf("  - 蒙哥马利算法\n\n");

    printf("注意:\n");
    printf("  - 仅用于学习目的\n");
    printf("  - 生产环境请使用专业库\n");
}

// 演示 2: 模幂运算
static void demo_modpow(void) {
    printf("\n=== 演示 2: 模幂运算 ===\n");

    printf("rsa_tiny_modpow 函数:\n");
    printf("  功能: 计算 (base^exp) %% mod\n");
    printf("  参数: base - 底数\n");
    printf("        exp - 指数\n");
    printf("        mod - 模数\n");
    printf("  返回: 计算结果\n\n");

    printf("示例:\n");
    printf("  计算: 3^5 %% 7\n");
    printf("  3^5 = 243\n");
    printf("  243 %% 7 = 5\n\n");

    printf("算法:\n");
    printf("  使用快速幂算法\n");
    printf("  时间复杂度: O(log exp)\n");
}

// 演示 3: 加密解密
static void demo_crypto(void) {
    printf("\n=== 演示 3: 加密解密 ===\n");

    printf("rsa_key_t 结构:\n");
    printf("  n: 模数 (p * q)\n");
    printf("  e: 公钥指数\n");
    printf("  d: 私钥指数\n");
    printf("  p: 素数因子 p\n");
    printf("  q: 素数因子 q\n\n");

    printf("rsa_tiny_encrypt 函数:\n");
    printf("  功能: RSA 加密\n");
    printf("  公式: ciphertext = plaintext^e %% n\n\n");

    printf("rsa_tiny_decrypt 函数:\n");
    printf("  功能: RSA 解密\n");
    printf("  公式: plaintext = ciphertext^d %% n\n");
}

// 演示 4: 算法变体
static void demo_algorithms(void) {
    printf("\n=== 演示 4: 算法变体 ===\n");

    printf("rsa_tiny_modpow_binary:\n");
    printf("  二进制快速幂算法\n");
    printf("  将指数转换为二进制\n");
    printf("  通过平方和乘法计算\n\n");

    printf("rsa_tiny_modpow_montgomery:\n");
    printf("  蒙哥马利算法\n");
    printf("  避免除法运算\n");
    printf("  适合大数运算\n\n");

    printf("性能对比:\n");
    printf("  二进制: 通用，实现简单\n");
    printf("  蒙哥马利: 大数时更快\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("rsa_config_t 结构:\n");
    printf("  use_optimized: 是否使用优化算法\n");
    printf("  check_inputs: 是否检查输入参数\n");
    printf("  use_montgomery: 是否使用蒙哥马利算法\n");
    printf("  max_bits: 最大位数\n");
    printf("  max_iterations: 最大迭代次数\n\n");

    printf("rsa_default_config 函数:\n");
    printf("  功能: 获取默认配置\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RSA_OK - 成功\n");
    printf("  RSA_ERROR_NULL_PTR - 空指针\n");
    printf("  RSA_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RSA_ERROR_MODULUS_TOO_SMALL - 模数过小\n");
    printf("  RSA_ERROR_EXPONENT_TOO_LARGE - 指数过大\n");
    printf("  RSA_ERROR_BASE_TOO_LARGE - 底数过大\n");
    printf("  RSA_ERROR_CALCULATION_FAILED - 计算失败\n");
    printf("  RSA_ERROR_OVERFLOW - 溢出错误\n");

    printf("\n安全提示:\n");
    printf("  - 仅用于学习\n");
    printf("  - 生产环境使用 OpenSSL\n");
    printf("  - 注意整数溢出\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 学习目的\n");
    printf("   - 理解 RSA 原理\n");
    printf("   - 学习模幂运算\n");
    printf("   - 算法实现练习\n\n");

    printf("2. 简单加密\n");
    printf("   - 小数据加密\n");
    printf("   - 演示程序\n");
    printf("   - 原型验证\n\n");

    printf("3. 嵌入式系统\n");
    printf("   - 资源受限环境\n");
    printf("   - 轻量级应用\n");
    printf("   - 教育设备\n\n");

    printf("4. 性能测试\n");
    printf("   - 算法基准测试\n");
    printf("   - 优化对比\n");
    printf("   - 教学演示\n\n");

    printf("5. 扩展开发\n");
    printf("   - 大数库基础\n");
    printf("  算法优化研究\n");
    printf("   - 密码学学习\n");
}

int main(void) {
    printf("========================================\n");
    printf("    RSA 精简版演示\n");
    printf("========================================\n");
    printf("    注意: 仅用于学习目的!\n");
    printf("========================================\n");

    demo_concept();
    demo_modpow();
    demo_crypto();
    demo_algorithms();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("警告: 请勿用于生产环境!\n");

    return 0;
}
