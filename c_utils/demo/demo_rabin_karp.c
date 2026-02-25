/**
 * Rabin-Karp 字符串匹配演示程序
 *
 * 功能：
 * - 滚动哈希匹配
 * - 多模式匹配
 * - 性能分析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/rabin_karp.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: Rabin-Karp 基本概念 ===\n");

    printf("Rabin-Karp 算法:\n\n");

    printf("核心思想:\n");
    printf("  - 使用滚动哈希快速比较\n");
    printf("  - 哈希匹配后再字符比较\n");
    printf("  - 平均时间复杂度 O(n+m)\n\n");

    printf("哈希计算:\n");
    printf("  hash = (d^(m-1)*txt[0] + ... + txt[m-1]) mod q\n");
    printf("  其中: d 是字符集大小, q 是大素数\n\n");

    printf("滚动哈希:\n");
    printf("  hash(i+1) = (d*(hash(i) - txt[i]*h) + txt[i+m]) mod q\n");
    printf("  其中: h = d^(m-1) mod q\n");
}

// 演示 2: 基本搜索函数
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本字符串搜索 ===\n");

    printf("rabin_karp_search 函数:\n");
    printf("  功能: 查找模式在文本中的位置\n");
    printf("  参数: text - 文本字符串\n");
    printf("        pattern - 模式字符串\n");
    printf("  返回: 匹配位置，未找到返回 -1\n\n");

    printf("示例:\n");
    printf("  const char *text = \"ABABDABACDABABCABAB\";\n");
    printf("  const char *pattern = \"ABABCABAB\";\n");
    printf("  int pos = rabin_karp_search(text, pattern);\n");
    printf("  // 返回: 10\n");
}

// 演示 3: 查找所有匹配
static void demo_find_all(void) {
    printf("\n=== 演示 3: 查找所有匹配 ===\n");

    printf("rabin_karp_find_all 函数:\n");
    printf("  功能: 查找所有匹配位置\n");
    printf("  参数: txt - 文本\n");
    printf("        txt_len - 文本长度\n");
    printf("        pat - 模式\n");
    printf("        pat_len - 模式长度\n");
    printf("        result - 结果输出\n");
    printf("        config - 配置选项\n");
    printf("        error - 错误码输出\n");
    printf("  返回: 是否成功\n\n");

    printf("rabin_karp_result_t 结构:\n");
    printf("  positions - 匹配位置数组\n");
    printf("  count - 匹配数量\n");
    printf("  capacity - 结果容量\n");
    printf("  overflow - 是否溢出\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    printf("rabin_karp_config_t 结构:\n");
    printf("  case_insensitive: 忽略大小写\n");
    printf("  binary_mode: 处理二进制数据\n");
    printf("  radix: 哈希基数 (通常为 256)\n");
    printf("  modulus: 哈希模数 (大素数)\n");
    printf("  max_matches: 最大匹配数\n\n");

    printf("配置说明:\n");
    printf("  - case_insensitive: 忽略大小写\n");
    printf("  - binary_mode: 处理二进制数据\n");
    printf("  - radix: 通常为 256 (字符集大小)\n");
    printf("  - modulus: 大素数，减少哈希冲突\n");
}

// 演示 5: 多模式匹配
static void demo_multi_pattern(void) {
    printf("\n=== 演示 5: 多模式匹配概念 ===\n");

    printf("Rabin-Karp 多模式匹配:\n\n");

    printf("基本思路:\n");
    printf("  1. 计算所有模式的哈希值\n");
    printf("  2. 维护模式哈希集合\n");
    printf("  3. 文本窗口哈希与集合比较\n\n");

    printf("应用场景:\n");
    printf("  - 病毒扫描\n");
    printf("  - 入侵检测\n");
    printf("  - 文本过滤\n\n");

    printf("优势:\n");
    printf("  - 同时检查多个模式\n");
    printf("  - 平均 O(n+m) 复杂度\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RABIN_KARP_OK - 成功\n");
    printf("  RABIN_KARP_ERROR_NULL_PTR - 空指针\n");
    printf("  RABIN_KARP_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RABIN_KARP_ERROR_PATTERN_TOO_LONG - 模式过长\n");
    printf("  RABIN_KARP_ERROR_TEXT_TOO_SHORT - 文本过短\n");
    printf("  RABIN_KARP_ERROR_HASH_OVERFLOW - 哈希溢出\n");

    printf("\n常见错误场景:\n");
    printf("  - 模式比文本长 -> TEXT_TOO_SHORT\n");
    printf("  - 哈希计算溢出 -> HASH_OVERFLOW\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 文本搜索\n");
    printf("   - 文件内容查找\n");
    printf("   - 日志分析\n");
    printf("   - 代码搜索\n\n");

    printf("2. 生物信息学\n");
    printf("   - DNA 序列匹配\n");
    printf("   - 蛋白质分析\n");
    printf("   - 基因组比对\n\n");

    printf("3. 网络安全\n");
    printf("   - 病毒特征检测\n");
    printf("   - 入侵检测系统\n");
    printf("   - 恶意代码扫描\n\n");

    printf("4. 数据去重\n");
    printf("   - 文件去重\n");
    printf("   - 相似度检测\n");
    printf("   - 抄袭检测\n\n");

    printf("5. 编译器\n");
    printf("   - 词法分析\n");
    printf("   - 关键字匹配\n");
    printf("   - 模式识别\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Rabin-Karp 字符串匹配演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_find_all();
    demo_config();
    demo_multi_pattern();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
