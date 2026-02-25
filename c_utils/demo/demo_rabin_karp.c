/**
 * Rabin-Karp 字符串搜索演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/rabin_karp.h"

static void demo_basic_search(void) {
    printf("\n=== 演示 1: 基本搜索 ===\n");

    const char *text = "The quick brown fox jumps over the lazy dog";
    const char *pattern = "fox";

    printf("文本: \"%s\"\n", text);
    printf("模式: \"%s\"\n", pattern);

    int pos = rabin_karp_search(text, pattern);
    if (pos >= 0) {
        printf("找到位置: %d\n", pos);
    } else {
        printf("未找到\n");
    }
}

static void demo_multiple_patterns(void) {
    printf("\n=== 演示 2: 多个模式搜索 ===\n");

    const char *text = "hello world";
    const char *patterns[] = {"hello", "world", "foo"};

    printf("文本: \"%s\"\n\n", text);

    for (int i = 0; i < 3; i++) {
        int pos = rabin_karp_search(text, patterns[i]);
        printf("搜索 \"%s\": ", patterns[i]);
        if (pos >= 0) {
            printf("在位置 %d 找到\n", pos);
        } else {
            printf("未找到\n");
        }
    }
}

static void demo_not_found(void) {
    printf("\n=== 演示 3: 未找到 ===\n");

    const char *text = "The quick brown fox";
    const char *pattern = "cat";

    printf("文本: \"%s\"\n", text);
    printf("模式: \"%s\"\n", pattern);

    int pos = rabin_karp_search(text, pattern);
    printf("结果: %s\n", pos >= 0 ? "找到" : "未找到");
}

int main(void) {
    printf("========================================\n");
    printf("    Rabin-Karp 字符串搜索演示\n");
    printf("========================================\n");

    demo_basic_search();
    demo_multiple_patterns();
    demo_not_found();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
