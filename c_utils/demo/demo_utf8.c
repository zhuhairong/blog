/**
 * @file demo_utf8.c
 * @brief UTF-8 字符串操作演示
 *
 * 本演示展示了 UTF-8 字符串的基本操作，包括：
 * - UTF-8 字符串验证
 * - UTF-8 字符串长度计算
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utf8.h"

// 演示 1: UTF-8 字符串验证
static void demo_validate_utf8(void) {
    printf("\n=== 演示 1: UTF-8 字符串验证 ===\n");

    const char *test_strings[] = {
        "Hello, world!",                // ASCII 字符串
        "你好，世界！",                    // 中文 UTF-8 字符串
        "こんにちは",                    // 日文 UTF-8 字符串
        "안녕하세요",                    // 韩文 UTF-8 字符串
        "Привет",                      // 俄文 UTF-8 字符串
        "السلام عليكم",                  // 阿拉伯文 UTF-8 字符串
        NULL
    };

    for (int i = 0; test_strings[i]; i++) {
        const char *str = test_strings[i];
        bool valid = utf8_is_valid(str);
        printf("字符串 '%s' 是 %s\n", str, valid ? "有效的 UTF-8" : "无效的 UTF-8");
    }
}

// 演示 2: UTF-8 字符串长度计算
static void demo_utf8_length(void) {
    printf("\n=== 演示 2: UTF-8 字符串长度计算 ===\n");

    const char *test_strings[] = {
        "Hello, world!",                // ASCII 字符串
        "你好，世界！",                    // 中文 UTF-8 字符串
        "こんにちは",                    // 日文 UTF-8 字符串
        "안녕하세요",                    // 韩文 UTF-8 字符串
        "Привет",                      // 俄文 UTF-8 字符串
        "السلام عليكم",                  // 阿拉伯文 UTF-8 字符串
        NULL
    };

    for (int i = 0; test_strings[i]; i++) {
        const char *str = test_strings[i];
        size_t byte_len = strlen(str);
        size_t char_len = utf8_strlen(str);
        printf("字符串 '%s'\n", str);
        printf("  字节长度: %zu\n", byte_len);
        printf("  字符长度: %zu\n", char_len);
    }
}

// 演示 3: UTF-8 字符串处理示例
static void demo_utf8_processing(void) {
    printf("\n=== 演示 3: UTF-8 字符串处理 ===\n");

    const char *utf8_str = "你好，UTF-8 世界！";
    printf("原始字符串: %s\n", utf8_str);
    printf("是否有效: %s\n", utf8_is_valid(utf8_str) ? "是" : "否");
    printf("字符长度: %zu\n", utf8_strlen(utf8_str));
    printf("字节长度: %zu\n", strlen(utf8_str));
}

// 演示 4: UTF-8 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 4: UTF-8 应用场景 ===\n");

    printf("1. 国际化应用:\n");
    printf("   - 多语言用户界面\n");
    printf("   - 多语言文档和内容\n");
    printf("   - 国际化域名 (IDN)\n");

    printf("\n2. 数据存储和传输:\n");
    printf("   - 数据库存储\n");
    printf("   - 网络协议\n");
    printf("   - 文件格式 (JSON, XML, etc.)\n");

    printf("\n3. 文本处理:\n");
    printf("   - 文本编辑器\n");
    printf("   - 搜索引擎\n");
    printf("   - 自然语言处理\n");

    printf("\n4. Web 开发:\n");
    printf("   - HTML5 和 CSS3\n");
    printf("   - JavaScript 字符串处理\n");
    printf("   - 响应式设计中的多语言支持\n");
}

int main(void) {
    printf("========================================\n");
    printf("    UTF-8 字符串操作演示程序\n");
    printf("========================================\n");

    demo_validate_utf8();
    demo_utf8_length();
    demo_utf8_processing();
    demo_applications();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}