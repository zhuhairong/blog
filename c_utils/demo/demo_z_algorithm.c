/**
 * Z 算法演示程序
 *
 * 功能：
 * - Z 数组计算
 * - 字符串模式匹配
 * - 查找所有匹配
 * - 最长公共前缀计算
 * - 批量 Z 数组计算
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/z_algorithm.h"

// 辅助函数：打印 Z 数组
static void print_z_array(const char *s, int *z, size_t len) {
    printf("  字符串: ");
    for (size_t i = 0; i < len; i++) printf(" %c ", s[i]);
    printf("\n");
    printf("  位置:   ");
    for (size_t i = 0; i < len; i++) printf("%2zu ", i);
    printf("\n");
    printf("  Z数组:  ");
    for (size_t i = 0; i < len; i++) printf("%2d ", z[i]);
    printf("\n");
}

// 演示 1: Z 数组基本计算
static void demo_basic_z(void) {
    printf("\n=== 演示 1: Z 数组基本计算 ===\n");

    const char *s = "aabcaabxaa";
    size_t len = strlen(s);
    int *z = malloc(len * sizeof(int));

    z_algorithm_compute(s, z);

    printf("输入字符串: \"%s\"\n", s);
    printf("Z 数组定义: Z[i] = s[i..] 与 s 的最长公共前缀长度\n\n");

    print_z_array(s, z, len);

    printf("\n解读:\n");
    printf("  Z[0] = 0 (约定，始终为 0)\n");
    for (size_t i = 1; i < len; i++) {
        if (z[i] > 0) {
            printf("  Z[%zu] = %d => s[%zu..] 与 s[0..] 的前 %d 个字符相同\n",
                   i, z[i], i, z[i]);
        }
    }

    free(z);
}

// 演示 2: 使用安全 API 计算 Z 数组
static void demo_safe_compute(void) {
    printf("\n=== 演示 2: 安全 API 计算 Z 数组 ===\n");

    z_algorithm_ctx_t *ctx = NULL;
    z_algorithm_config_t config = {
        .use_optimization = true,
        .check_bounds = true,
        .allow_empty_string = false,
        .max_string_length = 1024,
        .max_pattern_length = 256
    };

    z_algorithm_error_t err = z_algorithm_create(&ctx, &config);
    if (err != Z_ALGORITHM_OK) {
        printf("创建上下文失败: %s\n", z_algorithm_strerror(err));
        return;
    }

    const char *s = "abacabadabacaba";
    size_t len = strlen(s);
    int *z = malloc(len * sizeof(int));

    err = z_algorithm_compute_safe(ctx, s, len, z, len);
    if (err == Z_ALGORITHM_OK) {
        printf("输入: \"%s\"\n\n", s);
        print_z_array(s, z, len);

        // 分析 Z 数组
        printf("\n周期分析:\n");
        for (size_t i = 1; i < len; i++) {
            if (z[i] > 0 && (size_t)z[i] == len - i) {
                printf("  Z[%zu] = %d => 字符串存在长度为 %.0f 的前缀周期\n",
                       i, z[i], (double)i);
            }
        }
    } else {
        printf("计算失败: %s\n", z_algorithm_strerror(err));
    }

    free(z);
    z_algorithm_destroy(ctx);
}

// 演示 3: 模式匹配
static void demo_pattern_match(void) {
    printf("\n=== 演示 3: 模式匹配 ===\n");

    z_algorithm_ctx_t *ctx = NULL;
    z_algorithm_create(&ctx, NULL);

    const char *text = "GCATCGCAGAGAGTATACAGTACG";
    const char *pattern = "GCAGAGAG";
    size_t text_len = strlen(text);
    size_t pattern_len = strlen(pattern);

    printf("文本: \"%s\"\n", text);
    printf("模式: \"%s\"\n", pattern);

    size_t matches[20];
    size_t match_count = 0;
    z_algorithm_error_t err = z_algorithm_match(ctx, text, text_len, pattern, pattern_len,
                                                  matches, 20, &match_count);

    if (err == Z_ALGORITHM_OK || match_count > 0) {
        printf("\n找到 %zu 个匹配:\n", match_count);
        for (size_t i = 0; i < match_count && i < 20; i++) {
            printf("  位置 %zu: ", matches[i]);
            // 可视化
            for (size_t j = 0; j < matches[i]; j++) printf(" ");
            printf("%s\n", pattern);
        }
    } else {
        printf("未找到匹配: %s\n", z_algorithm_strerror(err));
    }

    z_algorithm_destroy(ctx);
}

// 演示 4: 查找所有匹配
static void demo_find_all(void) {
    printf("\n=== 演示 4: 查找所有匹配 ===\n");

    z_algorithm_ctx_t *ctx = NULL;
    z_algorithm_create(&ctx, NULL);

    const char *text = "ABABDABACDABABCABAB";
    const char *pattern = "ABAB";

    printf("文本: \"%s\"\n", text);
    printf("模式: \"%s\"\n\n", pattern);

    size_t *matches = NULL;
    size_t match_count = 0;
    z_algorithm_error_t err = z_algorithm_find_all_matches(ctx, text, strlen(text),
                                                             pattern, strlen(pattern),
                                                             &matches, &match_count);

    if (err == Z_ALGORITHM_OK && match_count > 0) {
        printf("找到 %zu 个匹配:\n", match_count);
        for (size_t i = 0; i < match_count; i++) {
            printf("  位置 %zu\n", matches[i]);
        }

        // 可视化
        printf("\n可视化:\n");
        printf("  %s\n", text);
        for (size_t j = 0; j < match_count; j++) {
            printf("  ");
            for (size_t k = 0; k < matches[j]; k++) printf(" ");
            printf("%s\n", pattern);
        }
    } else {
        printf("未找到匹配\n");
    }

    free(matches);
    z_algorithm_destroy(ctx);
}

// 演示 5: 最长公共前缀 (LCP)
static void demo_lcp(void) {
    printf("\n=== 演示 5: 最长公共前缀 (LCP) ===\n");

    struct {
        const char *s1;
        const char *s2;
    } pairs[] = {
        {"hello world", "hello there"},
        {"prefix_match", "prefix_test"},
        {"abc", "xyz"},
        {"完全相同的字符串", "完全相同的字符串"},
        {"", "nonempty"},
        {NULL, NULL}
    };

    printf("最长公共前缀 (LCP) 对比:\n\n");
    for (int i = 0; pairs[i].s1; i++) {
        size_t lcp = z_algorithm_lcp(pairs[i].s1, pairs[i].s2);
        printf("  s1: \"%s\"\n", pairs[i].s1);
        printf("  s2: \"%s\"\n", pairs[i].s2);
        printf("  LCP: %zu", lcp);
        if (lcp > 0) {
            printf(" (\"");
            for (size_t j = 0; j < lcp; j++) printf("%c", pairs[i].s1[j]);
            printf("\")");
        }
        printf("\n\n");
    }
}

// 演示 6: 批量 Z 数组计算
static void demo_batch(void) {
    printf("\n=== 演示 6: 批量 Z 数组计算 ===\n");

    z_algorithm_ctx_t *ctx = NULL;
    z_algorithm_create(&ctx, NULL);

    const char *strings[] = {
        "aaaaaa",
        "abcabcabc",
        "aabcaabxa"
    };
    size_t counts = 3;
    size_t lengths[3];
    int *z_arrays[3];
    size_t z_sizes[3];

    for (size_t i = 0; i < counts; i++) {
        lengths[i] = strlen(strings[i]);
    }

    z_algorithm_error_t err = z_algorithm_compute_batch(ctx, strings, lengths,
                                                          counts, z_arrays, z_sizes);

    if (err == Z_ALGORITHM_OK) {
        printf("批量计算了 %zu 个字符串的 Z 数组:\n\n", counts);
        for (size_t i = 0; i < counts; i++) {
            printf("字符串 %zu: \"%s\"\n", i + 1, strings[i]);
            printf("  Z 数组: [");
            for (size_t j = 0; j < z_sizes[i]; j++) {
                printf("%d", z_arrays[i][j]);
                if (j < z_sizes[i] - 1) printf(", ");
            }
            printf("]\n\n");
            free(z_arrays[i]);
        }
    } else {
        printf("批量计算失败: %s\n", z_algorithm_strerror(err));
    }

    z_algorithm_destroy(ctx);
}

// 演示 7: Z 算法用于查找重复模式
static void demo_repeat_detection(void) {
    printf("\n=== 演示 7: 使用 Z 算法检测重复模式 ===\n");

    const char *test_strings[] = {
        "abcabcabc",    // 周期为 3
        "aaaaaa",       // 周期为 1
        "abababab",     // 周期为 2
        "abcdefg",      // 无周期
        NULL
    };

    printf("检测字符串的重复周期:\n\n");

    for (int t = 0; test_strings[t]; t++) {
        const char *s = test_strings[t];
        size_t len = strlen(s);
        int *z = malloc(len * sizeof(int));
        z_algorithm_compute(s, z);

        printf("字符串: \"%s\"\n", s);
        bool found_period = false;

        for (size_t i = 1; i < len; i++) {
            if (z[i] > 0 && (size_t)z[i] == len - i) {
                printf("  → 周期为 %zu (模式重复 %zu 次)\n", i, len / i);
                found_period = true;
                break;
            }
        }

        if (!found_period) {
            printf("  → 未检测到完整周期\n");
        }

        free(z);
        printf("\n");
    }
}

// 演示 8: Z 算法与其他算法的比较
static void demo_comparison(void) {
    printf("\n=== 演示 8: Z 算法对比分析 ===\n");

    printf("Z 算法 vs KMP 算法:\n\n");
    printf("  相同点:\n");
    printf("    - 都是线性时间 O(n + m) 的字符串匹配算法\n");
    printf("    - 都需要预处理\n");
    printf("    - 都避免回溯\n\n");

    printf("  不同点:\n");
    printf("    - Z 算法: 预处理文本字符串，用 Z 数组描述\n");
    printf("    - KMP 算法: 预处理模式串，用 LPS 表描述\n");
    printf("    - Z 算法更适合多变模式的场景\n");
    printf("    - KMP 更适合单模式多次搜索的场景\n\n");

    printf("应用场景:\n");
    printf("  1. 文本编辑器搜索功能\n");
    printf("  2. DNA 序列比对\n");
    printf("  3. 数据压缩 (LZ77 族算法)\n");
    printf("  4. 字符串周期分析\n");
    printf("  5. 抄袭检测\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Z 算法演示\n");
    printf("========================================\n");

    demo_basic_z();
    demo_safe_compute();
    demo_pattern_match();
    demo_find_all();
    demo_lcp();
    demo_batch();
    demo_repeat_detection();
    demo_comparison();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
