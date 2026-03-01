/**
 * RSA 精简版演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../c_utils/rsa_tiny.h"

static void demo_modpow_basic(void) {
    printf("\n=== 演示 1: 基本模幂运算 ===\n");

    uint64_t base = 3, exp = 5, mod = 7;
    uint64_t result = rsa_tiny_modpow(base, exp, mod);
    printf("计算: %lu^%lu mod %lu = %lu\n", 
           (unsigned long)base, (unsigned long)exp, (unsigned long)mod, (unsigned long)result);
    printf("验证: 3^5 = 243, 243 mod 7 = %u\n", 243 % 7);

    result = rsa_tiny_modpow(2, 10, 1000);
    printf("\n2^10 mod 1000 = %lu\n", (unsigned long)result);

    result = rsa_tiny_modpow(7, 3, 10);
    printf("7^3 mod 10 = %lu\n", (unsigned long)result);
}

static void demo_large_modpow(void) {
    printf("\n=== 演示 2: 大数模幂运算 ===\n");

    uint64_t result = rsa_tiny_modpow(12345, 6789, 1000000007);
    printf("12345^6789 mod 1000000007 = %lu\n", (unsigned long)result);

    result = rsa_tiny_modpow(999999937, 1234567, 1000000007);
    printf("999999937^1234567 mod 1000000007 = %lu\n", (unsigned long)result);
}

static void demo_rsa_math(void) {
    printf("\n=== 演示 3: RSA 数学原理 ===\n");

    uint64_t p = 7, q = 11;
    uint64_t n = p * q;
    uint64_t phi = (p - 1) * (q - 1);
    uint64_t e = 7;
    uint64_t d = rsa_tiny_modpow(e, phi - 1, phi);

    printf("p=%lu, q=%lu, n=%lu, phi=%lu\n", 
           (unsigned long)p, (unsigned long)q, (unsigned long)n, (unsigned long)phi);
    printf("e=%lu (公钥), d=%lu (私钥)\n", (unsigned long)e, (unsigned long)d);

    uint64_t msg = 9;
    uint64_t enc = rsa_tiny_modpow(msg, e, n);
    uint64_t dec = rsa_tiny_modpow(enc, d, n);

    printf("\n明文: %lu -> 加密: %lu -> 解密: %lu\n", 
           (unsigned long)msg, (unsigned long)enc, (unsigned long)dec);
    printf("验证: %s\n", msg == dec ? "OK" : "FAIL");
}

static void demo_encrypt_decrypt(void) {
    printf("\n=== 演示 4: 加密解密 ===\n");

    uint64_t p = 61, q = 53;
    uint64_t n = p * q;
    uint64_t e = 17;
    uint64_t phi = (p - 1) * (q - 1);
    uint64_t d = rsa_tiny_modpow(e, phi - 1, phi);

    printf("密钥: p=%lu, q=%lu, n=%lu, e=%lu, d=%lu\n",
           (unsigned long)p, (unsigned long)q, (unsigned long)n, (unsigned long)e, (unsigned long)d);
    printf("最大明文: %lu\n", (unsigned long)n - 1);

    uint64_t plaintext = 65;
    uint64_t ciphertext = rsa_tiny_modpow(plaintext, e, n);
    uint64_t decrypted = rsa_tiny_modpow(ciphertext, d, n);

    printf("\n明文: %lu\n", (unsigned long)plaintext);
    printf("加密: %lu\n", (unsigned long)ciphertext);
    printf("解密: %lu\n", (unsigned long)decrypted);
    printf("结果: %s\n", plaintext == decrypted ? "OK" : "FAIL");
}

static void demo_classic_rsa(void) {
    printf("\n=== 演示 5: 经典 RSA 示例 ===\n");

    uint64_t n = 3233;
    uint64_t e = 17;
    uint64_t d = 2753;

    printf("经典示例: n=3233, e=17, d=2753\n");
    printf("测试多个明文:\n");

    int tests[] = {65, 100, 500, 1000, 2000};
    for (int i = 0; i < 5; i++) {
        uint64_t pt = tests[i];
        uint64_t ct = rsa_tiny_modpow(pt, e, n);
        uint64_t dt = rsa_tiny_modpow(ct, d, n);
        printf("  %4d -> %4lu -> %4lu : %s\n", 
               pt, (unsigned long)ct, (unsigned long)dt,
               pt == dt ? "OK" : "FAIL");
    }
}

static void demo_algorithm(void) {
    printf("\n=== 演示 6: 算法说明 ===\n");

    printf("rsa_tiny_modpow 使用快速幂算法 (二分幂)\n");
    printf("复杂度: O(log exp)\n\n");

    printf("示例: 计算 3^13 mod 100\n");
    printf("  13 = 1101 (二进制)\n");
    printf("  3^13 = 3^8 * 3^4 * 3^1\n");

    uint64_t result = rsa_tiny_modpow(3, 13, 100);
    printf("  结果: %lu\n", (unsigned long)result);
}

static void demo_security(void) {
    printf("\n=== 演示 7: 安全性说明 ===\n");

    printf("RSA 安全性基于大数分解困难性\n\n");

    printf("不安全的小模数:\n");
    printf("  n=15 (3*5), n=21 (3*7), n=35 (5*7)\n\n");

    printf("推荐密钥长度:\n");
    printf("  - 1024 位: 不推荐\n");
    printf("  - 2048 位: 推荐\n");
    printf("  - 4096 位: 高安全需求\n\n");

    printf("本库使用 64 位整数，仅供学习\n");
}

int main(void) {
    printf("========================================\n");
    printf("    RSA 精简版演示\n");
    printf("========================================\n");

    demo_modpow_basic();
    demo_large_modpow();
    demo_rsa_math();
    demo_encrypt_decrypt();
    demo_classic_rsa();
    demo_algorithm();
    demo_security();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("========================================\n");

    return 0;
}
