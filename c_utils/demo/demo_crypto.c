/**
 * @file demo_crypto.c
 * @brief 加密算法演示
 *
 * 本演示展示了常用加密算法的应用，包括：
 * - SHA-256 哈希计算
 * - MD5 哈希计算
 * - 十六进制编码
 * - 数据完整性验证
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/crypto.h"

// 打印十六进制字符串
static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

// 演示 1: SHA-256 基本计算
static void demo_sha256_basic(void) {
    printf("\n=== 演示 1: SHA-256 基本计算 ===\n");

    const char *test_strings[] = {
        "Hello, World!",
        "1234567890",
        "The quick brown fox jumps over the lazy dog",
        "",
        "abc"
    };
    int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("SHA-256 哈希计算:\n\n");
    for (int i = 0; i < num_strings; i++) {
        sha256_ctx_t ctx;
        uint8_t digest[32];
        
        sha256_init(&ctx);
        sha256_update(&ctx, (const uint8_t *)test_strings[i], strlen(test_strings[i]));
        sha256_final(&ctx, digest);
        
        printf("输入: \"%s\"\n", test_strings[i]);
        print_hex("SHA-256", digest, 32);
        printf("\n");
    }
}

// 演示 2: SHA-256 增量计算
static void demo_sha256_incremental(void) {
    printf("\n=== 演示 2: SHA-256 增量计算 ===\n");

    sha256_ctx_t ctx;
    uint8_t digest[32];
    
    sha256_init(&ctx);
    
    // 分块更新
    const char *chunks[] = {
        "Hello, ",
        "World",
        "!",
        " This is a test."
    };
    int num_chunks = sizeof(chunks) / sizeof(chunks[0]);
    
    printf("增量计算:\n");
    for (int i = 0; i < num_chunks; i++) {
        sha256_update(&ctx, (const uint8_t *)chunks[i], strlen(chunks[i]));
        printf("  添加 \"%s\"\n", chunks[i]);
    }
    
    sha256_final(&ctx, digest);
    printf("\n最终 SHA-256:\n");
    print_hex("  ", digest, 32);
    
    // 验证与一次性计算结果相同
    char full_string[256] = "";
    for (int i = 0; i < num_chunks; i++) {
        strcat(full_string, chunks[i]);
    }
    
    sha256_ctx_t ctx2;
    uint8_t digest2[32];
    sha256_init(&ctx2);
    sha256_update(&ctx2, (const uint8_t *)full_string, strlen(full_string));
    sha256_final(&ctx2, digest2);
    
    printf("\n直接计算:\n");
    print_hex("  ", digest2, 32);
    printf("\n结果一致: %s\n", memcmp(digest, digest2, 32) == 0 ? "是" : "否");
}

// 演示 3: MD5 计算
static void demo_md5(void) {
    printf("\n=== 演示 3: MD5 计算 ===\n");

    const char *test_strings[] = {
        "Hello, World!",
        "1234567890",
        "The quick brown fox jumps over the lazy dog",
        ""
    };
    int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("MD5 哈希计算:\n\n");
    for (int i = 0; i < num_strings; i++) {
        md5_ctx_t ctx;
        uint8_t digest[16];
        
        md5_init(&ctx);
        md5_update(&ctx, (const uint8_t *)test_strings[i], strlen(test_strings[i]));
        md5_final(&ctx, digest);
        
        printf("输入: \"%s\"\n", test_strings[i]);
        print_hex("MD5", digest, 16);
        printf("\n");
    }
}

// 演示 4: 十六进制编码
static void demo_hex_encoding(void) {
    printf("\n=== 演示 4: 十六进制编码 ===\n");

    uint8_t binary_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00, 0x01, 0xFF};
    size_t len = sizeof(binary_data);
    
    printf("二进制数据:\n");
    printf("  长度: %zu 字节\n", len);
    printf("  原始: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", binary_data[i]);
    }
    printf("\n");
    
    char hex_string[64];
    crypto_to_hex(binary_data, len, hex_string);
    
    printf("\n十六进制编码:\n");
    printf("  %s\n", hex_string);
    printf("  长度: %zu 字符\n", strlen(hex_string));
}

// 演示 5: 文件哈希
static void demo_file_hash(void) {
    printf("\n=== 演示 5: 文件哈希 ===\n");

    // 创建测试文件
    const char *test_file = "/tmp/crypto_test.txt";
    FILE *fp = fopen(test_file, "w");
    if (!fp) {
        printf("无法创建测试文件\n");
        return;
    }
    
    fprintf(fp, "This is a test file for hash calculation.\n");
    fprintf(fp, "Line 2: Some more text here.\n");
    fprintf(fp, "Line 3: End of file.\n");
    fclose(fp);
    
    printf("测试文件: %s\n\n", test_file);
    
    // 计算文件 SHA-256
    FILE *file = fopen(test_file, "rb");
    if (file) {
        sha256_ctx_t ctx;
        uint8_t digest[32];
        uint8_t buffer[4096];
        size_t bytes_read;
        
        sha256_init(&ctx);
        
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            sha256_update(&ctx, buffer, bytes_read);
        }
        
        sha256_final(&ctx, digest);
        fclose(file);
        
        print_hex("文件 SHA-256", digest, 32);
    }
    
    // 清理
    remove(test_file);
}

// 演示 6: 数据完整性验证
static void demo_integrity(void) {
    printf("\n=== 演示 6: 数据完整性验证 ===\n");

    const char *original_data = "Important message to verify";
    size_t len = strlen(original_data);
    
    // 计算原始哈希
    sha256_ctx_t ctx;
    uint8_t original_hash[32];
    
    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t *)original_data, len);
    sha256_final(&ctx, original_hash);
    
    printf("原始数据: \"%s\"\n", original_data);
    print_hex("原始哈希", original_hash, 32);
    
    // 验证未修改的数据
    printf("\n场景 1: 数据未修改\n");
    sha256_ctx_t ctx2;
    uint8_t hash2[32];
    sha256_init(&ctx2);
    sha256_update(&ctx2, (const uint8_t *)original_data, len);
    sha256_final(&ctx2, hash2);
    
    bool match = (memcmp(original_hash, hash2, 32) == 0);
    printf("  哈希匹配: %s\n", match ? "是 (数据完整)" : "否");
    
    // 验证修改后的数据
    printf("\n场景 2: 数据被修改\n");
    char modified_data[256];
    strcpy(modified_data, original_data);
    modified_data[10] = 'X';  // 修改一个字符
    
    sha256_ctx_t ctx3;
    uint8_t hash3[32];
    sha256_init(&ctx3);
    sha256_update(&ctx3, (const uint8_t *)modified_data, len);
    sha256_final(&ctx3, hash3);
    
    print_hex("修改后哈希", hash3, 32);
    match = (memcmp(original_hash, hash3, 32) == 0);
    printf("  哈希匹配: %s\n", match ? "是" : "否 (数据已被修改)");
}

// 演示 7: 哈希算法比较
static void demo_comparison(void) {
    printf("\n=== 演示 7: 哈希算法比较 ===\n");

    const char *test_data = "The quick brown fox jumps over the lazy dog";
    size_t len = strlen(test_data);
    
    printf("测试数据: \"%s\"\n\n", test_data);
    
    // MD5
    md5_ctx_t md5_ctx;
    uint8_t md5_digest[16];
    md5_init(&md5_ctx);
    md5_update(&md5_ctx, (const uint8_t *)test_data, len);
    md5_final(&md5_ctx, md5_digest);
    
    printf("MD5:\n");
    print_hex("  ", md5_digest, 16);
    printf("  输出长度: 128 位\n");
    printf("  安全性: 已破解，不推荐用于安全场景\n\n");
    
    // SHA-256
    sha256_ctx_t sha_ctx;
    uint8_t sha_digest[32];
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, (const uint8_t *)test_data, len);
    sha256_final(&sha_ctx, sha_digest);
    
    printf("SHA-256:\n");
    print_hex("  ", sha_digest, 32);
    printf("  输出长度: 256 位\n");
    printf("  安全性: 目前安全，推荐使用\n");
}

// 演示 8: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 8: 性能测试 ===\n");

    size_t sizes[] = {1024, 10240, 102400, 1048576};  // 1KB, 10KB, 100KB, 1MB
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const int iterations = 1000;

    printf("哈希算法性能比较 (%d 次迭代):\n\n", iterations);
    printf("%-10s %-15s %-15s\n", "数据大小", "MD5 (MB/s)", "SHA-256 (MB/s)");
    printf("-------------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        size_t size = sizes[i];
        char *data = malloc(size);
        if (!data) continue;

        // 填充数据
        for (size_t j = 0; j < size; j++) {
            data[j] = (char)(j % 256);
        }

        // MD5 测试
        clock_t start = clock();
        for (int j = 0; j < iterations; j++) {
            md5_ctx_t ctx;
            uint8_t digest[16];
            md5_init(&ctx);
            md5_update(&ctx, (const uint8_t *)data, size);
            md5_final(&ctx, digest);
        }
        clock_t end = clock();
        double md5_time = (double)(end - start) / CLOCKS_PER_SEC;
        double md5_speed = (size * iterations / (1024.0 * 1024.0)) / md5_time;

        // SHA-256 测试
        start = clock();
        for (int j = 0; j < iterations; j++) {
            sha256_ctx_t ctx;
            uint8_t digest[32];
            sha256_init(&ctx);
            sha256_update(&ctx, (const uint8_t *)data, size);
            sha256_final(&ctx, digest);
        }
        end = clock();
        double sha_time = (double)(end - start) / CLOCKS_PER_SEC;
        double sha_speed = (size * iterations / (1024.0 * 1024.0)) / sha_time;

        if (size < 1024) {
            printf("%-10zu %-15.2f %-15.2f\n", size, md5_speed, sha_speed);
        } else if (size < 1024 * 1024) {
            printf("%-10.1fKB %-15.2f %-15.2f\n", size / 1024.0, md5_speed, sha_speed);
        } else {
            printf("%-10.1fMB %-15.2f %-15.2f\n", size / (1024.0 * 1024.0), md5_speed, sha_speed);
        }

        free(data);
    }
}

// 演示 9: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 9: 应用场景 ===\n");

    printf("哈希算法的应用场景:\n\n");

    printf("1. 数据完整性:\n");
    printf("   - 文件下载校验\n");
    printf("   - 数据备份验证\n");
    printf("   - 传输错误检测\n\n");

    printf("2. 数字签名:\n");
    printf("   - 软件发布签名\n");
    printf("   - 文档认证\n");
    printf("   - 代码签名\n\n");

    printf("3. 密码存储:\n");
    printf("   - 用户密码哈希\n");
    printf("   - API 密钥存储\n");
    printf("   - 结合盐值使用\n\n");

    printf("4. 数据去重:\n");
    printf("   - 重复文件检测\n");
    printf("   - 块级去重\n");
    printf("   - 缓存索引\n");
}

// 演示 10: 安全注意事项
static void demo_security(void) {
    printf("\n=== 演示 10: 安全注意事项 ===\n");

    printf("哈希算法安全指南:\n\n");

    printf("1. 算法选择:\n");
    printf("   - 避免使用 MD5/SHA-1 用于安全场景\n");
    printf("   - 推荐使用 SHA-256 或更高\n");
    printf("   - 关注算法破解进展\n\n");

    printf("2. 密码存储:\n");
    printf("   - 使用专门密码哈希算法 (bcrypt, Argon2)\n");
    printf("   - 必须添加随机盐值\n");
    printf("   - 使用足够迭代次数\n\n");

    printf("3. 碰撞攻击:\n");
    printf("   - MD5 已被破解，可构造碰撞\n");
    printf("   - SHA-1 理论上可碰撞\n");
    printf("   - SHA-256 目前安全\n\n");

    printf("4. 长度扩展:\n");
    printf("   - Merkle-Damgard 结构存在此问题\n");
    printf("   - 使用 HMAC 避免此问题\n");
    printf("   - 或使用 SHA-3 系列算法\n");
}

int main(void) {
    printf("========================================\n");
    printf("    加密算法演示程序\n");
    printf("========================================\n");

    demo_sha256_basic();
    demo_sha256_incremental();
    demo_md5();
    demo_hex_encoding();
    demo_file_hash();
    demo_integrity();
    demo_comparison();
    demo_performance();
    demo_use_cases();
    demo_security();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
