/**
 * KMP 字符串匹配演示程序
 *
 * 功能：
 * - 单模式匹配
 * - 多模式匹配
 * - 部分匹配表构建
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/kmp.h"

// 演示 1: 基本匹配
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本字符串匹配 ===\n");

    const char *text = "ABC ABCDAB ABCDABCDABDE";
    const char *pattern = "ABCDABD";

    printf("文本:    %s\n", text);
    printf("模式:    %s\n\n", pattern);

    int pos = kmp_search(text, pattern);

    if (pos >= 0) {
        printf("匹配位置: %d\n", pos);
        printf("匹配结果: ");
        for (int i = 0; i < pos; i++) printf(" ");
        printf("%s\n", pattern);
    } else {
        printf("未找到匹配\n");
    }
}

// 演示 2: 多个匹配
static void demo_multiple_matches(void) {
    printf("\n=== 演示 2: 多个匹配 ===\n");

    const char *text = "ABABDABACDABABCABAB";
    const char *pattern = "ABAB";

    printf("文本:    %s\n", text);
    printf("模式:    %s\n\n", pattern);

    kmp_error_t error;
    size_t matches[10];
    size_t count = kmp_search_all(text, strlen(text), pattern, strlen(pattern),
                                   matches, 10, &error);

    printf("找到 %zu 个匹配:\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  匹配 %zu: 位置 %zu\n", i + 1, matches[i]);
    }

    // 可视化
    printf("\n可视化:\n  %s\n  ", text);
    for (size_t i = 0; i < count; i++) {
        for (size_t j = 0; j < matches[i]; j++) printf(" ");
        printf("%s", pattern);
        if (i < count - 1) printf("\n  ");
    }
    printf("\n");
}

// 演示 3: 部分匹配表 (LPS)
static void demo_lps(void) {
    printf("\n=== 演示 3: 部分匹配表 (LPS) ===\n");

    const char *patterns[] = {
        "AAAA",
        "ABCDE",
        "AABAACAABAA",
        "ABACABAB",
        NULL
    };

    printf("LPS (Longest Prefix Suffix) 表:\n");
    printf("对于模式串的每个位置，记录最长相同前后缀长度\n\n");

    for (int p = 0; patterns[p]; p++) {
        const char *pattern = patterns[p];
        int len = strlen(pattern);
        int *lps = malloc(len * sizeof(int));

        kmp_error_t error;
        kmp_build_lps(pattern, len, lps, &error);

        printf("模式: %s\n", pattern);
        printf("  位置: ");
        for (int i = 0; i < len; i++) printf("%3d", i);
        printf("\n");
        printf("  字符: ");
        for (int i = 0; i < len; i++) printf("%3c", pattern[i]);
        printf("\n");
        printf("  LPS:  ");
        for (int i = 0; i < len; i++) printf("%3d", lps[i]);
        printf("\n\n");

        free(lps);
    }
}

// 演示 4: 算法优势
static void demo_advantages(void) {
    printf("\n=== 演示 4: KMP 算法优势 ===\n");

    printf("与朴素算法对比:\n\n");

    printf("朴素算法:\n");
    printf("  - 最坏情况: O(m × n)\n");
    printf("  - 每次不匹配都回溯到模式开头\n");
    printf("  - 重复比较已匹配的字符\n\n");

    printf("KMP 算法:\n");
    printf("  - 最坏情况: O(m + n)\n");
    printf("  - 利用 LPS 表避免回溯\n");
    printf("  - 每个字符只比较一次\n\n");

    printf("示例: 文本 = \"AAAAAAAAAB\", 模式 = \"AAAAB\"\n");
    printf("  朴素算法: 需要约 40 次比较\n");
    printf("  KMP 算法: 只需要约 14 次比较\n");
}

// 演示 5: 大小写敏感选项
static void demo_case_options(void) {
    printf("\n=== 演示 5: 大小写选项 ===\n");

    const char *text = "Hello World";
    const char *pattern = "world";

    printf("文本:   %s\n", text);
    printf("模式:   %s\n\n", pattern);

    kmp_error_t error;
    kmp_config_t config;
    kmp_get_default_config(&config);

    // 大小写敏感
    config.case_sensitive = true;
    int pos1 = kmp_search_with_config(text, strlen(text), pattern, strlen(pattern),
                                       &config, &error);
    printf("大小写敏感: %s\n", pos1 >= 0 ? "找到" : "未找到");

    // 大小写不敏感
    config.case_sensitive = false;
    int pos2 = kmp_search_with_config(text, strlen(text), pattern, strlen(pattern),
                                       &config, &error);
    printf("大小写不敏感: %s (位置 %d)\n", pos2 >= 0 ? "找到" : "未找到", pos2);
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 文本编辑器\n");
    printf("   - 查找功能\n");
    printf("   - 搜索替换\n");
    printf("\n");

    printf("2. 生物信息学\n");
    printf("   - DNA 序列匹配\n");
    printf("   - 蛋白质序列分析\n");
    printf("\n");

    printf("3. 入侵检测\n");
    printf("   - 模式匹配检测\n");
    printf("   - 病毒特征码扫描\n");
    printf("\n");

    printf("4. 数据压缩\n");
    printf("   - LZ77/LZ78 算法\n");
    printf("   - 重复模式查找\n");
    printf("\n");

    printf("5. 日志分析\n");
    printf("   - 特定模式搜索\n");
    printf("   - 错误模式匹配\n");
}

// 演示 7: 复杂度分析
static void demo_complexity(void) {
    printf("\n=== 演示 7: 复杂度分析 ===\n");

    printf("时间复杂度:\n");
    printf("  预处理 (构建 LPS): O(m)\n");
    printf("  搜索: O(n)\n");
    printf("  总计: O(m + n)\n");
    printf("  (m = 模式长度, n = 文本长度)\n\n");

    printf("空间复杂度:\n");
    printf("  LPS 表: O(m)\n\n");

    printf("特点:\n");
    printf("  - 线性时间复杂度\n");
    printf("  - 不需要回溯文本指针\n");
    printf("  - 适合流式数据\n");
}

int main(void) {
    printf("========================================\n");
    printf("    KMP 字符串匹配演示\n");
    printf("========================================\n");

    demo_basic();
    demo_multiple_matches();
    demo_lps();
    demo_advantages();
    demo_case_options();
    demo_applications();
    demo_complexity();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
