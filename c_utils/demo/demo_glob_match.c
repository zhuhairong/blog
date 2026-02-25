/**
 * Glob 匹配演示程序
 *
 * 功能：
 * - 简单通配符匹配
 * - 配置化匹配
 * - 批量匹配
 * - 模式验证
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/glob_match.h"

// 演示 1: 简单匹配
static void demo_simple(void) {
    printf("\n=== 演示 1: 简单通配符匹配 ===\n");

    glob_match_error_t error;

    struct {
        const char *pattern;
        const char *text;
        bool expected;
    } cases[] = {
        {"hello", "hello", true},
        {"hello", "world", false},
        {"h?llo", "hello", true},
        {"h?llo", "hallo", true},
        {"h*o", "hello", true},
        {"h*o", "ho", true},
        {"*test*", "this is a test", true},
        {"file.*", "file.txt", true},
        {"file.*", "file", false},
        {NULL, NULL, false}
    };

    printf("简单匹配测试:\n");
    for (int i = 0; cases[i].pattern; i++) {
        bool result = glob_match_simple(cases[i].pattern, cases[i].text,
                                        GLOB_MATCH_FLAG_NONE, &error);
        const char *mark = (result == cases[i].expected) ? "✓" : "✗";
        printf("  %s \"%s\" match \"%s\" = %s\n",
               mark, cases[i].pattern, cases[i].text,
               result ? "true" : "false");
    }
}

// 演示 2: 大小写敏感选项
static void demo_case_sensitivity(void) {
    printf("\n=== 演示 2: 大小写敏感选项 ===\n");

    glob_match_error_t error;

    printf("大小写敏感 (默认):\n");
    printf("  \"Hello\" match \"hello\" = %s\n",
           glob_match_simple("Hello", "hello", GLOB_MATCH_FLAG_NONE, &error) ? "true" : "false");

    printf("\n大小写不敏感:\n");
    printf("  \"Hello\" match \"hello\" = %s\n",
           glob_match_simple("Hello", "hello", GLOB_MATCH_FLAG_CASE_INSENSITIVE, &error) ? "true" : "false");
    printf("  \"FILE.TXT\" match \"file.txt\" = %s\n",
           glob_match_simple("FILE.TXT", "file.txt", GLOB_MATCH_FLAG_CASE_INSENSITIVE, &error) ? "true" : "false");
}

// 演示 3: 自定义配置
static void demo_config(void) {
    printf("\n=== 演示 3: 自定义匹配配置 ===\n");

    glob_match_error_t error;
    glob_match_config_t config = glob_match_default_config();

    printf("默认配置:\n");
    printf("  单字符通配符: '%c'\n", config.wildcard_single);
    printf("  多字符通配符: '%c'\n", config.wildcard_any);
    printf("  转义字符: '%c'\n", config.escape_char);

    printf("\n使用默认配置匹配:\n");
    printf("  \"data?.txt\" match \"data1.txt\" = %s\n",
           glob_match_config("data?.txt", "data1.txt", &config, &error) ? "true" : "false");

    // 自定义配置
    config.wildcard_single = '#';
    config.wildcard_any = '@';

    printf("\n自定义配置 (#=单字符, @=多字符):\n");
    printf("  \"data#.txt\" match \"data1.txt\" = %s\n",
           glob_match_config("data#.txt", "data1.txt", &config, &error) ? "true" : "false");
    printf("  \"@.txt\" match \"anything.txt\" = %s\n",
           glob_match_config("@.txt", "anything.txt", &config, &error) ? "true" : "false");
}

// 演示 4: 批量匹配
static void demo_batch(void) {
    printf("\n=== 演示 4: 批量匹配 ===\n");

    glob_match_error_t error;

    const char *patterns[] = {"*.c", "*.h", "Makefile"};
    const char *files[] = {"main.c", "utils.h", "Makefile", "readme.txt"};
    int n_patterns = sizeof(patterns) / sizeof(patterns[0]);
    int n_files = sizeof(files) / sizeof(files[0]);

    printf("批量匹配 %d 个文件:\n", n_files);
    for (int i = 0; i < n_files; i++) {
        printf("  %s:\n", files[i]);
        for (int j = 0; j < n_patterns; j++) {
            bool match = glob_match_simple(patterns[j], files[i],
                                           GLOB_MATCH_FLAG_NONE, &error);
            printf("    %s %s\n", match ? "✓" : "✗", patterns[j]);
        }
    }
}

// 演示 5: 模式匹配技巧
static void demo_patterns(void) {
    printf("\n=== 演示 5: 模式匹配技巧 ===\n");

    glob_match_error_t error;

    struct {
        const char *pattern;
        const char *text;
        const char *desc;
    } cases[] = {
        {"*.txt", "document.txt", "匹配所有 txt 文件"},
        {"data?.csv", "data1.csv", "匹配 data 加单个字符"},
        {"[0-9]*", "123abc", "数字开头"},
        {"*.{c,h}", "main.c", "匹配 c 或 h 扩展名"},
        {"*.{c,h}", "main.h", "匹配 c 或 h 扩展名"},
        {NULL, NULL, NULL}
    };

    printf("常用模式示例:\n");
    for (int i = 0; cases[i].pattern; i++) {
        bool result = glob_match_simple(cases[i].pattern, cases[i].text,
                                        GLOB_MATCH_FLAG_NONE, &error);
        printf("  %s\n", cases[i].desc);
        printf("    \"%s\" match \"%s\" = %s\n\n",
               cases[i].pattern, cases[i].text, result ? "是" : "否");
    }
}

// 演示 6: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 文件名过滤\n");
    printf("   *.log     - 日志文件\n");
    printf("   data???.csv - 特定格式数据文件\n");
    printf("\n");

    printf("2. 路由匹配\n");
    printf("   /user/*   - 用户相关路径\n");
    printf("   /api/v?/* - API 版本路径\n");
    printf("\n");

    printf("3. 输入验证\n");
    printf("   [a-z]*    - 小写字母开头\n");
    printf("   *[0-9]    - 数字结尾\n");
    printf("\n");

    printf("4. 配置匹配\n");
    printf("   include:* - 包含规则\n");
    printf("   exclude:* - 排除规则\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Glob 匹配演示\n");
    printf("========================================\n");

    demo_simple();
    demo_case_sensitivity();
    demo_config();
    demo_batch();
    demo_patterns();
    demo_use_cases();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
