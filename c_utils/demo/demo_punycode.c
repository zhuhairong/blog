/**
 * Punycode 编码演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/punycode.h"

static void demo_basic_encode(void) {
    printf("\n=== 演示 1: 基本 Punycode 编码 ===\n");

    uint32_t input1[] = {0x4E2D, 0x56FD};
    char output1[64];
    size_t output_len1 = 64;

    bool ok = punycode_encode(input1, 2, output1, &output_len1);
    if (ok) {
        printf("输入: 中国 (U+4E2D, U+56FD)\n");
        printf("Punycode 编码: %s\n", output1);
        printf("输出长度: %zu\n", output_len1);
    } else {
        printf("编码失败\n");
    }

    uint32_t input2[] = {0x65E5, 0x672C, 0x56FD};
    char output2[64];
    size_t output_len2 = 64;

    ok = punycode_encode(input2, 3, output2, &output_len2);
    if (ok) {
        printf("\n输入: 日本 (U+65E5, U+672C, U+56FD)\n");
        printf("Punycode 编码: %s\n", output2);
    }
}

static void demo_multiple_strings(void) {
    printf("\n=== 演示 2: 多个字符串编码 ===\n");

    struct {
        uint32_t chars[10];
        size_t len;
        const char *desc;
    } tests[] = {
        {{0x41, 0x42, 0x43}, 3, "ABC (ASCII)"},
        {{0x4E2D}, 1, "中"},
        {{0x4E2D, 0x56FD}, 2, "中国"},
        {{0x0}, 0, "空字符串"}
    };

    for (int i = 0; i < 4; i++) {
        char output[64];
        size_t output_len = 64;
        bool ok = punycode_encode(tests[i].chars, tests[i].len, output, &output_len);
        printf("  %s: ", tests[i].desc);
        if (ok) {
            printf("%s (len=%zu)\n", output, output_len);
        } else {
            printf("失败\n");
        }
    }
}

static void demo_various_unicode(void) {
    printf("\n=== 演示 3: 各种 Unicode 字符 ===\n");

    struct {
        uint32_t chars[5];
        size_t len;
        const char *desc;
    } tests[] = {
        {{0x0639}, 1, "阿拉伯文 ع"},
        {{0x0430}, 1, "俄文 а"},
        {{0x0939}, 1, "印地文 ह"},
        {{0x20AC}, 1, "欧元 €"},
        {{0x4E8C}, 1, "日文 二"}
    };

    for (int i = 0; i < 5; i++) {
        char output[64];
        size_t output_len = 64;
        bool ok = punycode_encode(tests[i].chars, tests[i].len, output, &output_len);
        printf("  %s: ", tests[i].desc);
        if (ok) {
            printf("%s\n", output);
        } else {
            printf("失败\n");
        }
    }
}

int main(void) {
    printf("========================================\n");
    printf("    Punycode 编码演示\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_multiple_strings();
    demo_various_unicode();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
