/**
 * Glob 模式匹配演示程序
 * 
 * 功能：
 * - 字符串模式匹配
 * - 文件系统 glob 匹配
 * - 批量匹配
 * - 模式验证
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/glob.h"

// 演示 1: 基本模式匹配
static void demo_basic_match(void) {
    printf("\n=== 演示 1: 基本模式匹配 ===\n");
    
    glob_error_t error;
    
    // 测试用例
    struct {
        const char *pattern;
        const char *text;
        bool expected;
    } test_cases[] = {
        {"*.txt", "file.txt", true},
        {"*.txt", "file.c", false},
        {"data?.log", "data1.log", true},
        {"data?.log", "data12.log", false},
        {"[abc]*", "apple", true},
        {"[abc]*", "banana", true},
        {"[abc]*", "grape", false},
        {"*.{c,h}", "test.c", true},
        {"*.{c,h}", "test.h", true},
        {"*.{c,h}", "test.cpp", false},
        {NULL, NULL, false}
    };
    
    printf("模式匹配测试:\n");
    for (int i = 0; test_cases[i].pattern; i++) {
        bool result = glob_match(test_cases[i].pattern, test_cases[i].text, 
                                  GLOB_FLAG_NONE, &error);
        const char *status = (result == test_cases[i].expected) ? "✓" : "✗";
        printf("  %s \"%s\" vs \"%s\" -> %s (期望: %s)\n",
               status,
               test_cases[i].pattern,
               test_cases[i].text,
               result ? "匹配" : "不匹配",
               test_cases[i].expected ? "匹配" : "不匹配");
    }
}

// 演示 2: 通配符说明
static void demo_wildcards(void) {
    printf("\n=== 演示 2: 通配符说明 ===\n");
    
    printf("Glob 通配符:\n\n");
    
    printf("* (星号): 匹配任意数量的任意字符\n");
    printf("  *.txt     -> 匹配所有 .txt 文件\n");
    printf("  data*     -> 匹配 data 开头的文件\n");
    printf("  *data*    -> 匹配包含 data 的文件\n");
    printf("\n");
    
    printf("? (问号): 匹配单个任意字符\n");
    printf("  data?.log -> 匹配 data1.log, dataA.log\n");
    printf("  ???       -> 匹配任意 3 个字符\n");
    printf("\n");
    
    printf("[...] (字符类): 匹配括号内的任意字符\n");
    printf("  [abc]     -> 匹配 a, b, 或 c\n");
    printf("  [0-9]     -> 匹配任意数字\n");
    printf("  [a-z]     -> 匹配任意小写字母\n");
    printf("  [!abc]    -> 匹配非 a, b, c 的字符\n");
    printf("\n");
    
    printf("{...} (大括号扩展): 匹配多个模式之一\n");
    printf("  *.{c,h}   -> 匹配 .c 或 .h 文件\n");
    printf("  {jpg,png} -> 匹配 jpg 或 png\n");
}

// 演示 3: 文件系统 glob
static void demo_filesystem(void) {
    printf("\n=== 演示 3: 文件系统 Glob ===\n");
    
    glob_error_t error;
    glob_t result;
    glob_config_t config = glob_default_config();
    
    printf("文件系统匹配示例:\n\n");
    
    // 匹配 /tmp 目录下的文件
    printf("匹配 /tmp/*.txt:\n");
    if (glob("/tmp/*.txt", &config, &result, &error)) {
        printf("  找到 %zu 个匹配:\n", result.gl_pathc);
        for (size_t i = 0; i < result.gl_pathc && i < 5; i++) {
            printf("    %s\n", result.gl_pathv[i]);
        }
        if (result.gl_pathc > 5) {
            printf("    ... (还有 %zu 个)\n", result.gl_pathc - 5);
        }
        glob_free(&result);
    } else {
        printf("  无匹配或出错\n");
    }
    
    // 匹配当前目录
    printf("\n匹配 .c 文件:\n");
    if (glob("*.c", &config, &result, &error)) {
        printf("  找到 %zu 个 .c 文件\n", result.gl_pathc);
        glob_free(&result);
    } else {
        printf("  无匹配\n");
    }
}

// 演示 4: 批量匹配
static void demo_batch_match(void) {
    printf("\n=== 演示 4: 批量匹配 ===\n");
    
    glob_error_t error;
    
    const char *patterns[] = {
        "*.c",
        "*.h",
        "*.txt",
        "Makefile",
        "README*"
    };
    int num_patterns = sizeof(patterns) / sizeof(patterns[0]);
    
    const char *test_files[] = {
        "main.c",
        "utils.h",
        "config.txt",
        "Makefile",
        "README.md",
        "test.py"
    };
    int num_files = sizeof(test_files) / sizeof(test_files[0]);
    
    printf("批量匹配测试:\n");
    printf("  模式: ");
    for (int i = 0; i < num_patterns; i++) {
        printf("%s ", patterns[i]);
    }
    printf("\n\n");
    
    for (int i = 0; i < num_files; i++) {
        bool matches[5];
        glob_match_batch(patterns, test_files[i], num_patterns, 
                         GLOB_FLAG_NONE, matches, &error);
        
        printf("  %s:\n", test_files[i]);
        for (int j = 0; j < num_patterns; j++) {
            if (matches[j]) {
                printf("    匹配 %s\n", patterns[j]);
            }
        }
    }
}

// 演示 5: 匹配选项
static void demo_flags(void) {
    printf("\n=== 演示 5: 匹配选项 ===\n");
    
    printf("Glob 标志:\n\n");
    
    printf("GLOB_FLAG_NONE:\n");
    printf("  默认匹配行为\n");
    printf("\n");
    
    printf("GLOB_FLAG_CASE_INSENSITIVE:\n");
    printf("  大小写不敏感匹配\n");
    printf("  *.TXT 可以匹配 file.txt\n");
    printf("\n");
    
    printf("GLOB_FLAG_PATHNAME:\n");
    printf("  * 不匹配 /\n");
    printf("  用于路径匹配\n");
    printf("\n");
    
    printf("GLOB_FLAG_NOESCAPE:\n");
    printf("  禁用转义字符\n");
    printf("\n");
    
    printf("GLOB_FLAG_PERIOD:\n");
    printf("  * 不匹配以 . 开头的文件\n");
    printf("  需要显式匹配 .\n");
}

// 演示 6: 实际应用
static void demo_applications(void) {
    printf("\n=== 演示 6: 实际应用场景 ===\n");
    
    printf("1. 文件过滤\n");
    printf("   ls *.txt          - 列出所有文本文件\n");
    printf("   rm *.log          - 删除所有日志文件\n");
    printf("   cp src/*.c dest/  - 复制所有源文件\n");
    printf("\n");
    
    printf("2. 构建系统\n");
    printf("   SOURCES = *.c     - 自动收集源文件\n");
    printf("   HEADERS = *.h     - 自动收集头文件\n");
    printf("\n");
    
    printf("3. 日志处理\n");
    printf("   app.2024-*.log    - 匹配 2024 年的日志\n");
    printf("   *.log.[0-9]       - 匹配轮转日志\n");
    printf("\n");
    
    printf("4. 配置文件\n");
    printf("   include *.conf    - 包含所有配置文件\n");
    printf("   exclude *.bak     - 排除备份文件\n");
    printf("\n");
    
    printf("5. 输入验证\n");
    printf("   [a-zA-Z0-9]+      - 验证用户名\n");
    printf("   *.@(jpg|png|gif)  - 验证图片格式\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Glob 模式匹配演示\n");
    printf("========================================\n");
    
    demo_basic_match();
    demo_wildcards();
    demo_filesystem();
    demo_batch_match();
    demo_flags();
    demo_applications();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
