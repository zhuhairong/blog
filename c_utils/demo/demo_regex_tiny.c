/**
 * 简易正则表达式演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/regex_tiny.h"

static void demo_basic_match(void) {
    printf("\n=== 演示 1: 基本匹配 ===\n");

    struct {
        const char *pattern;
        const char *text;
    } tests[] = {
        {"hello", "hello world"},
        {"world", "hello world"},
        {"^hello", "hello world"},
        {"world$", "hello world"},
        {"^hello world$", "hello world"},
        {"h.llo", "hello"},
        {"h.llo", "hallo"},
        {"h.llo", "hxllo"},
        {"hel*o", "heo"},
        {"hel*o", "helo"},
        {"hel*o", "hello"},
        {"hel*o", "helllo"},
        {"a.*b", "aXXXb"},
        {"a.*b", "ab"},
    };

    int n = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < n; i++) {
        bool match = regex_match(tests[i].pattern, tests[i].text);
        printf("  \"%s\" =~ /%s/ -> %s\n",
               tests[i].text, tests[i].pattern, match ? "匹配" : "不匹配");
    }
}

static void demo_wildcards(void) {
    printf("\n=== 演示 2: 通配符 ===\n");

    printf("点号 (.) 匹配任意字符:\n");
    const char *dot_tests[] = {"cat", "bat", "rat", "car", "bar"};
    for (int i = 0; i < 5; i++) {
        bool match = regex_match(".at", dot_tests[i]);
        printf("  \".at\" =~ \"%s\" -> %s\n", dot_tests[i], match ? "匹配" : "不匹配");
    }

    printf("\n星号 (*) 匹配零或多个:\n");
    struct {
        const char *pattern;
        const char *text;
    } star_tests[] = {
        {"ab*c", "ac"},
        {"ab*c", "abc"},
        {"ab*c", "abbc"},
        {"ab*c", "abbbc"},
        {"a*b*c", "abc"},
        {"a*b*c", "bc"},
        {"a*b*c", "aac"},
    };
    int n = sizeof(star_tests) / sizeof(star_tests[0]);
    for (int i = 0; i < n; i++) {
        bool match = regex_match(star_tests[i].pattern, star_tests[i].text);
        printf("  \"%s\" =~ /%s/ -> %s\n",
               star_tests[i].text, star_tests[i].pattern, match ? "匹配" : "不匹配");
    }
}

static void demo_anchors(void) {
    printf("\n=== 演示 3: 锚点 ===\n");

    printf("脱字符 (^) 匹配开头:\n");
    const char *start_tests[] = {"hello world", "world hello", "hello"};
    for (int i = 0; i < 3; i++) {
        bool match = regex_match("^hello", start_tests[i]);
        printf("  \"^hello\" =~ \"%s\" -> %s\n", start_tests[i], match ? "匹配" : "不匹配");
    }

    printf("\n美元符 ($) 匹配结尾:\n");
    const char *end_tests[] = {"hello world", "world hello", "hello"};
    for (int i = 0; i < 3; i++) {
        bool match = regex_match("hello$", end_tests[i]);
        printf("  \"hello$\" =~ \"%s\" -> %s\n", end_tests[i], match ? "匹配" : "不匹配");
    }

    printf("\n组合锚点 (精确匹配):\n");
    bool match = regex_match("^hello$", "hello");
    printf("  \"^hello$\" =~ \"hello\" -> %s\n", match ? "匹配" : "不匹配");
    match = regex_match("^hello$", "hello world");
    printf("  \"^hello$\" =~ \"hello world\" -> %s\n", match ? "匹配" : "不匹配");
}

static void demo_email_like(void) {
    printf("\n=== 演示 4: 简易邮箱匹配 ===\n");

    const char *pattern = "^[a-z]*@[a-z]*.[a-z]*$";

    const char *emails[] = {
        "test@example.com",
        "user@domain.org",
        "a@b.c",
        "invalid",
        "@example.com",
        "user@",
    };

    printf("模式: %s\n\n", pattern);
    for (int i = 0; i < 6; i++) {
        bool match = regex_match(pattern, emails[i]);
        printf("  \"%s\" -> %s\n", emails[i], match ? "匹配" : "不匹配");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    简易正则表达式演示\n");
    printf("========================================\n");

    demo_basic_match();
    demo_wildcards();
    demo_anchors();
    demo_email_like();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
