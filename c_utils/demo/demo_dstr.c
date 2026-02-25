/**
 * 动态字符串 (dstr) 演示程序
 * 
 * 功能：
 * - 动态字符串创建和销毁
 * - 字符串拼接和修改
 * - 子串操作和查找替换
 * - 实用工具函数
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/dstr.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");
    
    // 从字符串创建
    printf("从字符串创建:\n");
    dstr s1 = dstr_new("Hello, World!");
    printf("  dstr_new(\"Hello, World!\") = \"%s\"\n", s1);
    printf("  长度: %zu, 容量: %zu\n", dstr_len(s1), dstr_capacity(s1));
    
    // 从长度创建
    printf("\n从长度创建:\n");
    dstr s2 = dstr_new_len("Binary\x00Data", 11);
    printf("  dstr_new_len(\"Binary\\x00Data\", 11)\n");
    printf("  长度: %zu\n", dstr_len(s2));
    
    // 空字符串
    printf("\n空字符串:\n");
    dstr s3 = dstr_new_empty();
    printf("  dstr_new_empty() = \"%s\"\n", s3);
    printf("  是否为空: %s\n", dstr_is_empty(s3) ? "是" : "否");
    
    // 清理和释放
    dstr_clear(s1);
    printf("\n清理后:\n");
    printf("  dstr_clear(s1), 长度: %zu\n", dstr_len(s1));
    
    dstr_free(s1);
    dstr_free(s2);
    dstr_free(s3);
}

// 演示 2: 字符串拼接
static void demo_append(void) {
    printf("\n=== 演示 2: 字符串拼接 ===\n");
    
    dstr s = dstr_new("Hello");
    printf("初始: \"%s\"\n", s);
    
    // 追加字符串
    s = dstr_append(s, ", ");
    printf("append(\", \"): \"%s\"\n", s);
    
    s = dstr_append(s, "World");
    printf("append(\"World\"): \"%s\"\n", s);
    
    // 追加字符
    s = dstr_append_char(s, '!');
    printf("append_char('!'): \"%s\"\n", s);
    
    // 格式化追加
    s = dstr_append(s, " ");
    s = dstr_append_fmt(s, "Number: %d, Pi: %.2f", 42, 3.14159);
    printf("append_fmt(...): \"%s\"\n", s);
    
    // 前置
    dstr s2 = dstr_new("World");
    printf("\n前置操作:\n");
    printf("  初始: \"%s\"\n", s2);
    s2 = dstr_prepend(s2, "Hello, ");
    printf("  prepend(\"Hello, \"): \"%s\"\n", s2);
    
    dstr_free(s);
    dstr_free(s2);
}

// 演示 3: 子串操作
static void demo_substr(void) {
    printf("\n=== 演示 3: 子串操作 ===\n");
    
    dstr s = dstr_new("Hello, World! Welcome to C.");
    printf("原始字符串: \"%s\"\n", s);
    
    // 提取子串
    dstr sub1 = dstr_substr(s, 7, 5);
    printf("substr(7, 5): \"%s\"\n", sub1);
    
    dstr sub2 = dstr_substr(s, 0, 5);
    printf("substr(0, 5): \"%s\"\n", sub2);
    
    // 复制
    dstr copy = dstr_dup(s);
    printf("\ndup(): \"%s\"\n", copy);
    
    // 比较
    printf("\n比较:\n");
    printf("  strcmp(\"abc\", \"def\"): %d\n", dstr_cmp(dstr_new("abc"), dstr_new("def")));
    printf("  strcmp(\"ABC\", \"abc\"): %d\n", dstr_cmp(dstr_new("ABC"), dstr_new("abc")));
    printf("  casecmp(\"ABC\", \"abc\"): %d\n", dstr_casecmp(dstr_new("ABC"), dstr_new("abc")));
    
    // 前缀后缀检查
    printf("\n前缀后缀检查:\n");
    printf("  starts_with(\"Hello\"): %s\n", 
           dstr_starts_with(s, "Hello") ? "是" : "否");
    printf("  ends_with(\"C.\"): %s\n", 
           dstr_ends_with(s, "C.") ? "是" : "否");
    printf("  contains(\"World\"): %s\n", 
           dstr_contains(s, "World") ? "是" : "否");
    
    dstr_free(s);
    dstr_free(sub1);
    dstr_free(sub2);
    dstr_free(copy);
}

// 演示 4: 查找和替换
static void demo_find_replace(void) {
    printf("\n=== 演示 4: 查找和替换 ===\n");
    
    dstr s = dstr_new("The quick brown fox jumps over the lazy dog. The fox is quick.");
    printf("原始字符串:\n  \"%s\"\n", s);
    
    // 查找
    printf("\n查找:\n");
    ssize_t pos1 = dstr_find(s, "fox");
    printf("  find(\"fox\"): %zd\n", pos1);
    
    ssize_t pos2 = dstr_rfind(s, "fox");
    printf("  rfind(\"fox\"): %zd\n", pos2);
    
    ssize_t pos3 = dstr_find(s, "cat");
    printf("  find(\"cat\"): %zd (未找到)\n", pos3);
    
    // 替换
    printf("\n替换:\n");
    dstr replaced = dstr_replace(s, "fox", "cat");
    printf("  replace(\"fox\", \"cat\"):\n    \"%s\"\n", replaced);
    
    dstr replaced2 = dstr_replace(s, "quick", "slow");
    printf("  replace(\"quick\", \"slow\"):\n    \"%s\"\n", replaced2);
    
    dstr_free(s);
    dstr_free(replaced);
    dstr_free(replaced2);
}

// 演示 5: 实用工具
static void demo_utils(void) {
    printf("\n=== 演示 5: 实用工具 ===\n");
    
    // 修剪
    dstr s1 = dstr_new("   Hello, World!   ");
    printf("修剪:\n");
    printf("  原始: \"%s\"\n", s1);
    dstr_trim(s1, " ");
    printf("  trim: \"%s\"\n", s1);
    
    dstr s2 = dstr_new("---Hello---");
    dstr_trim(s2, "-");
    printf("  trim(\"---Hello---\", \"-\"): \"%s\"\n", s2);
    
    // 大小写转换
    dstr s3 = dstr_new("Hello, World!");
    printf("\n大小写转换:\n");
    printf("  原始: \"%s\"\n", s3);
    dstr_tolower(s3);
    printf("  tolower: \"%s\"\n", s3);
    
    dstr_toupper(s3);
    printf("  toupper: \"%s\"\n", s3);
    
    // 反转
    dstr s4 = dstr_new("Hello");
    printf("\n反转:\n");
    printf("  原始: \"%s\"\n", s4);
    dstr_reverse(s4);
    printf("  reverse: \"%s\"\n", s4);
    
    dstr_free(s1);
    dstr_free(s2);
    dstr_free(s3);
    dstr_free(s4);
}

// 演示 6: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能对比 ===\n");
    
    printf("dstr vs 普通字符串:\n");
    printf("  dstr 优势:\n");
    printf("    - 自动内存管理\n");
    printf("    - 动态扩容\n");
    printf("    - 避免缓冲区溢出\n");
    printf("    - 方便的拼接操作\n");
    printf("\n");
    printf("  使用示例:\n");
    printf("    // 普通 C 字符串\n");
    printf("    char buf[256];\n");
    printf("    strcpy(buf, \"Hello\");\n");
    printf("    strcat(buf, \", \");\n");
    printf("    strcat(buf, \"World\");\n");
    printf("    // 需要手动管理缓冲区大小\n");
    printf("\n");
    printf("    // dstr\n");
    printf("    dstr s = dstr_new(\"Hello\");\n");
    printf("    s = dstr_append(s, \", \");\n");
    printf("    s = dstr_append(s, \"World\");\n");
    printf("    // 自动扩容，无需担心缓冲区\n");
    printf("    dstr_free(s);\n");
}

int main(void) {
    printf("========================================\n");
    printf("    动态字符串 (dstr) 演示\n");
    printf("========================================\n");
    
    demo_basic();
    demo_append();
    demo_substr();
    demo_find_replace();
    demo_utils();
    demo_performance();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
