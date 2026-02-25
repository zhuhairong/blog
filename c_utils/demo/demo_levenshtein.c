/**
 * Levenshtein 编辑距离演示程序
 *
 * 功能：
 * - 编辑距离计算
 * - 字符串相似度
 * - 拼写检查应用
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/levenshtein.h"

// 演示 1: 基本编辑距离
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本编辑距离 ===\n");

    struct {
        const char *s1;
        const char *s2;
        const char *desc;
    } cases[] = {
        {"kitten", "sitting", "经典示例"},
        {"sunday", "saturday", "较长字符串"},
        {"book", "back", "替换为主"},
        {"", "abc", "空字符串"},
        {"same", "same", "相同字符串"},
        {NULL, NULL, NULL}
    };

    printf("编辑距离计算:\n\n");

    for (int i = 0; cases[i].s1; i++) {
        size_t dist = levenshtein_distance(cases[i].s1, cases[i].s2);
        printf("%s:\n", cases[i].desc);
        printf("  \"%s\" -> \"%s\"\n", cases[i].s1, cases[i].s2);
        printf("  编辑距离: %zu\n\n", dist);
    }
}

// 演示 2: 相似度计算
static void demo_similarity(void) {
    printf("\n=== 演示 2: 字符串相似度 ===\n");

    struct {
        const char *s1;
        const char *s2;
    } cases[] = {
        {"hello", "hello"},
        {"hello", "helo"},
        {"hello", "hallo"},
        {"hello", "world"},
        {"computer", "computar"},
        {NULL, NULL}
    };

    printf("相似度 (0.0-1.0, 1.0 表示完全相同):\n\n");
    printf("  %-15s %-15s %-12s %-12s\n", "字符串1", "字符串2", "距离", "相似度");
    printf("  %-15s %-15s %-12s %-12s\n", "---------------", "---------------", "------------", "------------");

    for (int i = 0; cases[i].s1; i++) {
        size_t dist = levenshtein_distance(cases[i].s1, cases[i].s2);
        double sim = levenshtein_similarity(cases[i].s1, cases[i].s2);
        printf("  %-15s %-15s %-12zu %-12.2f\n",
               cases[i].s1, cases[i].s2, dist, sim);
    }
}

// 演示 3: 编辑操作类型
static void demo_operations(void) {
    printf("\n=== 演示 3: 编辑操作类型 ===\n");

    printf("三种基本编辑操作:\n\n");

    printf("1. 插入 (Insertion)\n");
    printf("   \"cat\" -> \"cart\"\n");
    printf("   在 'c' 后插入 'r'\n\n");

    printf("2. 删除 (Deletion)\n");
    printf("   \"cart\" -> \"cat\"\n");
    printf("   删除 'r'\n\n");

    printf("3. 替换 (Substitution)\n");
    printf("   \"cat\" -> \"cut\"\n");
    printf("   将 'a' 替换为 'u'\n\n");

    printf("示例: \"kitten\" -> \"sitting\"\n");
    printf("  1. kitten -> sitten (k->s, 替换)\n");
    printf("  2. sitten -> sittin (e->i, 替换)\n");
    printf("  3. sittin -> sitting (插入 g)\n");
    printf("  总距离: 3\n");
}

// 演示 4: 拼写检查应用
static void demo_spell_check(void) {
    printf("\n=== 演示 4: 拼写检查应用 ===\n");

    const char *dictionary[] = {
        "apple", "apply", "apart", "appear",
        "banana", "bandana", "band",
        "computer", "compute", "commute",
        NULL
    };

    const char *misspelled = "aple";

    printf("拼写检查:\n");
    printf("  输入: \"%s\"\n\n", misspelled);

    printf("  词典建议 (按相似度排序):\n");
    printf("  %-15s %-12s %-12s\n", "单词", "距离", "相似度");
    printf("  %-15s %-12s %-12s\n", "---------------", "------------", "------------");

    // 简单排序展示
    struct {
        const char *word;
        size_t dist;
        double sim;
    } results[10];
    int count = 0;

    for (int i = 0; dictionary[i] && count < 10; i++) {
        results[count].word = dictionary[i];
        results[count].dist = levenshtein_distance(misspelled, dictionary[i]);
        results[count].sim = levenshtein_similarity(misspelled, dictionary[i]);
        count++;
    }

    // 按距离排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (results[j].dist < results[i].dist) {
                struct { const char *w; size_t d; double s; } tmp;
                tmp.w = results[i].word; tmp.d = results[i].dist; tmp.s = results[i].sim;
                results[i].word = results[j].word; results[i].dist = results[j].dist; results[i].sim = results[j].sim;
                results[j].word = tmp.w; results[j].dist = tmp.d; results[j].sim = tmp.s;
            }
        }
    }

    for (int i = 0; i < count && i < 5; i++) {
        printf("  %-15s %-12zu %-12.2f\n",
               results[i].word, results[i].dist, results[i].sim);
    }
}

// 演示 5: DNA 序列比对
static void demo_dna(void) {
    printf("\n=== 演示 5: DNA 序列比对 ===\n");

    const char *seq1 = "ACGTACGT";
    const char *seq2 = "ACGTAGGT";

    printf("DNA 序列比对:\n");
    printf("  序列 1: %s\n", seq1);
    printf("  序列 2: %s\n", seq2);

    size_t dist = levenshtein_distance(seq1, seq2);
    double sim = levenshtein_similarity(seq1, seq2);

    printf("\n  编辑距离: %zu\n", dist);
    printf("  相似度: %.2f%%\n", sim * 100);

    printf("\n差异位置:\n");
    for (size_t i = 0; i < strlen(seq1) && i < strlen(seq2); i++) {
        if (seq1[i] != seq2[i]) {
            printf("  位置 %zu: %c -> %c\n", i, seq1[i], seq2[i]);
        }
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 拼写检查\n");
    printf("   - 自动纠错\n");
    printf("   - 搜索建议\n");
    printf("\n");

    printf("2. DNA 序列分析\n");
    printf("   - 基因比对\n");
    printf("   - 进化分析\n");
    printf("\n");

    printf("3. 语音识别\n");
    printf("   - 音素比对\n");
    printf("   - 相似发音匹配\n");
    printf("\n");

    printf("4.  plagiarism 检测\n");
    printf("   - 文本相似度\n");
    printf("   - 代码比对\n");
    printf("\n");

    printf("5. 模糊搜索\n");
    printf("   - 数据库模糊查询\n");
    printf("   - 日志分析\n");
}

// 演示 7: 复杂度分析
static void demo_complexity(void) {
    printf("\n=== 演示 7: 复杂度分析 ===\n");

    printf("标准算法:\n");
    printf("  时间: O(m × n)\n");
    printf("  空间: O(m × n)\n");
    printf("  (m, n 为两字符串长度)\n\n");

    printf("优化算法:\n");
    printf("  时间: O(m × n)\n");
    printf("  空间: O(min(m, n))\n");
    printf("  (只保留两行)\n\n");

    printf("实际应用建议:\n");
    printf("  - 短字符串: 标准算法\n");
    printf("  - 长字符串: 优化算法\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Levenshtein 编辑距离演示\n");
    printf("========================================\n");

    demo_basic();
    demo_similarity();
    demo_operations();
    demo_spell_check();
    demo_dna();
    demo_applications();
    demo_complexity();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
