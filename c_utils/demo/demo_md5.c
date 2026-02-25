/**
 * MD5 哈希演示程序
 *
 * 功能：
 * - 字符串 MD5 计算
 * - 文件 MD5 计算
 * - 增量哈希计算
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/md5.h"

// 打印 MD5 摘要
static void print_md5(const uint8_t digest[16]) {
    for (int i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
}

// 演示 1: 字符串 MD5
static void demo_string(void) {
    printf("\n=== 演示 1: 字符串 MD5 ===\n");

    struct {
        const char *str;
        const char *desc;
    } cases[] = {
        {"", "空字符串"},
        {"hello", "简单字符串"},
        {"The quick brown fox jumps over the lazy dog", "长字符串"},
        {"123456", "数字字符串"},
        {NULL, NULL}
    };

    printf("计算 MD5 哈希:\n\n");

    for (int i = 0; cases[i].str; i++) {
        uint8_t digest[16];
        md5_error_t error = md5_string(cases[i].str, digest);

        if (error == MD5_OK) {
            printf("%-20s: \"%s\"\n", cases[i].desc, cases[i].str);
            printf("  MD5: ");
            print_md5(digest);
            printf("\n\n");
        }
    }
}

// 演示 2: 十六进制输出
static void demo_hex(void) {
    printf("\n=== 演示 2: 十六进制字符串输出 ===\n");

    const char *str = "hello world";
    char hex[33];

    md5_error_t error = md5_string_hex(str, hex, sizeof(hex));

    if (error == MD5_OK) {
        printf("字符串: \"%s\"\n", str);
        printf("MD5: %s\n", hex);
    }
}

// 演示 3: 增量计算
static void demo_incremental(void) {
    printf("\n=== 演示 3: 增量 MD5 计算 ===\n");

    printf("分块计算 MD5:\n\n");

    md5_ctx_t ctx;
    md5_init(&ctx);

    // 分多次更新
    const char *part1 = "Hello ";
    const char *part2 = "World";
    const char *part3 = "!";

    printf("  更新 1: \"%s\"\n", part1);
    md5_update(&ctx, (const uint8_t*)part1, strlen(part1));

    printf("  更新 2: \"%s\"\n", part2);
    md5_update(&ctx, (const uint8_t*)part2, strlen(part2));

    printf("  更新 3: \"%s\"\n", part3);
    md5_update(&ctx, (const uint8_t*)part3, strlen(part3));

    uint8_t digest[16];
    md5_final(&ctx, digest);

    printf("\n  最终 MD5: ");
    print_md5(digest);
    printf("\n");

    // 验证与一次性计算相同
    uint8_t verify[16];
    md5_string("Hello World!", verify);

    printf("  验证结果: ");
    bool match = true;
    for (int i = 0; i < 16; i++) {
        if (digest[i] != verify[i]) {
            match = false;
            break;
        }
    }
    printf("%s\n", match ? "匹配" : "不匹配");
}

// 演示 4: 文件 MD5
static void demo_file(void) {
    printf("\n=== 演示 4: 文件 MD5 ===\n");

    const char *filename = "/tmp/demo_md5_test.txt";

    // 创建测试文件
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "This is a test file for MD5 calculation.\n");
        fprintf(fp, "It contains multiple lines.\n");
        fclose(fp);

        printf("测试文件: %s\n", filename);

        uint8_t digest[16];
        md5_error_t error;

        error = md5_file(filename, digest, &error);

        if (error == MD5_OK) {
            printf("文件 MD5: ");
            print_md5(digest);
            printf("\n");
        } else {
            printf("计算失败: %d\n", error);
        }

        // 清理
        remove(filename);
    }
}

// 演示 5: MD5 特性
static void demo_properties(void) {
    printf("\n=== 演示 5: MD5 特性 ===\n");

    printf("1. 固定长度\n");
    printf("   - 无论输入多长，输出总是 128 位 (16 字节)\n");
    printf("   - 十六进制表示为 32 个字符\n\n");

    printf("2. 雪崩效应\n");
    printf("   - 微小变化导致完全不同的哈希值\n\n");

    const char *s1 = "hello";
    const char *s2 = "Hello";

    uint8_t d1[16], d2[16];
    md5_string(s1, d1);
    md5_string(s2, d2);

    printf("   \"%s\": ", s1);
    print_md5(d1);
    printf("\n");
    printf("   \"%s\": ", s2);
    print_md5(d2);
    printf("\n\n");

    printf("3. 不可逆\n");
    printf("   - 无法从哈希值反推原始数据\n");
    printf("   - 单向函数\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 文件完整性校验\n");
    printf("   - 下载文件验证\n");
    printf("   - 备份完整性检查\n");
    printf("\n");

    printf("2. 数据去重\n");
    printf("   - 重复文件检测\n");
    printf("   - 块级去重\n");
    printf("\n");

    printf("3. 密码存储 (不推荐)\n");
    printf("   - 历史遗留系统\n");
    printf("   - 应使用 bcrypt/Argon2\n");
    printf("\n");

    printf("4. 数字签名\n");
    printf("   - 消息摘要\n");
    printf("   - 证书验证\n");
    printf("\n");

    printf("5. 缓存键生成\n");
    printf("   - 数据唯一标识\n");
    printf("   - 快速查找\n");
}

// 演示 7: 安全性说明
static void demo_security(void) {
    printf("\n=== 演示 7: 安全性说明 ===\n");

    printf("MD5 的安全问题:\n\n");

    printf("1. 碰撞攻击\n");
    printf("   - 2004 年发现碰撞\n");
    printf("   - 可以构造相同 MD5 的不同文件\n\n");

    printf("2. 不推荐用于:\n");
    printf("   - 密码存储\n");
    printf("   - 数字签名\n");
    printf("   - SSL 证书\n\n");

    printf("3. 替代方案:\n");
    printf("   - SHA-256 (推荐)\n");
    printf("   - SHA-3\n");
    printf("   - BLAKE2/BLAKE3\n\n");

    printf("4. 仍可用于:\n");
    printf("   - 非安全场景的文件校验\n");
    printf("   - 数据去重\n");
    printf("   - 缓存键\n");
}

int main(void) {
    printf("========================================\n");
    printf("    MD5 哈希演示\n");
    printf("========================================\n");

    demo_string();
    demo_hex();
    demo_incremental();
    demo_file();
    demo_properties();
    demo_applications();
    demo_security();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
