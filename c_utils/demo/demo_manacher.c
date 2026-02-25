/**
 * Manacher 算法演示程序
 *
 * 功能：
 * - 最长回文子串查找
 * - 所有回文子串统计
 * - 回文判断
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/manacher.h"

// 演示 1: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 1: 最长回文子串 ===\n");

    struct {
        const char *str;
        const char *desc;
    } cases[] = {
        {"babad", "示例 1"},
        {"cbbd", "示例 2"},
        {"a", "单字符"},
        {"racecar", "完整回文"},
        {"abcba", "奇数长度回文"},
        {"abba", "偶数长度回文"},
        {NULL, NULL}
    };

    printf("查找最长回文子串:\n\n");

    for (int i = 0; cases[i].str; i++) {
        int len = manacher_longest_palindrome(cases[i].str);
        printf("%s: \"%s\"\n", cases[i].desc, cases[i].str);
        printf("  最长回文长度: %d\n\n", len);
    }
}

// 演示 2: 详细结果
static void demo_detailed(void) {
    printf("\n=== 演示 2: 详细结果 ===\n");

    const char *str = "babad";

    printf("字符串: \"%s\"\n\n", str);

    manacher_config_t config;
    manacher_get_default_config(&config);

    size_t length, start;
    manacher_error_t error = manacher_longest_palindrome_ex(str, strlen(str),
                                                             &length, &start, &config);

    if (error == MANACHER_OK) {
        printf("最长回文子串:\n");
        printf("  起始位置: %zu\n", start);
        printf("  长度: %zu\n", length);
        printf("  子串: \"");
        for (size_t i = 0; i < length; i++) {
            printf("%c", str[start + i]);
        }
        printf("\"\n");
    }
}

// 演示 3: 查找所有回文
static void demo_find_all(void) {
    printf("\n=== 演示 3: 查找所有回文子串 ===\n");

    const char *str = "aaa";
    printf("字符串: \"%s\"\n\n", str);

    size_t *positions = NULL;
    size_t *lengths = NULL;
    manacher_error_t error;

    size_t count = manacher_find_all(str, strlen(str), &positions, &lengths, 100, &error);

    if (error == MANACHER_OK) {
        printf("找到 %zu 个回文子串:\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  位置 %zu, 长度 %zu: \"", positions[i], lengths[i]);
            for (size_t j = 0; j < lengths[i]; j++) {
                printf("%c", str[positions[i] + j]);
            }
            printf("\"\n");
        }

        free(positions);
        free(lengths);
    }
}

// 演示 4: 回文判断
static void demo_is_palindrome(void) {
    printf("\n=== 演示 4: 回文判断 ===\n");

    struct {
        const char *str;
        bool expected;
    } cases[] = {
        {"racecar", true},
        {"madam", true},
        {"hello", false},
        {"A man a plan a canal Panama", false},  // 有空格
        {"", true},  // 空字符串也是回文
        {NULL, false}
    };

    printf("回文判断 (大小写敏感):\n\n");

    for (int i = 0; cases[i].str; i++) {
        bool result = manacher_is_palindrome(cases[i].str, strlen(cases[i].str), true);
        printf("\"%s\": %s (预期: %s)\n",
               cases[i].str,
               result ? "是回文" : "不是回文",
               cases[i].expected ? "是" : "否");
    }
}

// 演示 5: 算法原理
static void demo_algorithm(void) {
    printf("\n=== 演示 5: Manacher 算法原理 ===\n");

    printf("核心思想:\n");
    printf("  1. 预处理: 在字符间插入特殊字符 (#)\n");
    printf("     \"aba\" -> \"#a#b#a#\"\n");
    printf("     统一处理奇偶长度回文\n\n");

    printf("  2. 维护回文半径数组 P[]\n");
    printf("     P[i] 表示以 i 为中心的回文半径\n\n");

    printf("  3. 利用对称性加速\n");
    printf("     维护最右回文边界 C 和 R\n");
    printf("     利用对称点信息减少重复计算\n\n");

    printf("时间复杂度: O(n)\n");
    printf("空间复杂度: O(n)\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. DNA 序列分析\n");
    printf("   - 查找回文序列\n");
    printf("   - 限制性内切酶识别\n");
    printf("\n");

    printf("2. 文本处理\n");
    printf("   - 回文检测\n");
    printf("   - 文本特征提取\n");
    printf("\n");

    printf("3. 生物信息学\n");
    printf("   - 蛋白质结构分析\n");
    printf("   - 序列比对\n");
    printf("\n");

    printf("4. 字符串处理\n");
    printf("   - 模式识别\n");
    printf("   - 数据压缩\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Manacher 算法演示\n");
    printf("========================================\n");

    demo_basic();
    demo_detailed();
    demo_find_all();
    demo_is_palindrome();
    demo_algorithm();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
