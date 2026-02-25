/**
 * 正则表达式演示程序
 *
 * 功能：
 * - 模式匹配
 * - 通配符支持
 * - 简单正则语法
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../c_utils/regex_tiny.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 正则表达式基本概念 ===\n");

    printf("正则表达式:\n\n");

    printf("支持的元字符:\n");
    printf("  .  - 匹配任意单个字符\n");
    printf("  *  - 匹配前一个字符 0 次或多次\n");
    printf("  +  - 匹配前一个字符 1 次或多次\n");
    printf("  ?  - 匹配前一个字符 0 次或 1 次\n");
    printf("  ^  - 匹配字符串开头\n");
    printf("  $  - 匹配字符串结尾\n");
    printf("  [] - 字符类\n");
    printf("  () - 分组\n");
    printf("  |  - 或\n\n");

    printf("转义字符:\n");
    printf("  \\. - 匹配字面量 .\n");
    printf("  \\* - 匹配字面量 *\n");
    printf("  \\\\ - 匹配字面量 \\\\n");
}

// 演示 2: 基本匹配
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本匹配 ===\n");

    printf("regex_match 函数:\n");
    printf("  功能: 检查文本是否匹配模式\n");
    printf("  参数: pattern - 正则表达式模式\n");
    printf("        text - 要匹配的文本\n");
    printf("  返回: 是否匹配\n\n");

    printf("示例:\n");
    printf("  pattern: \"hello\"\n");
    printf("  text: \"hello world\"\n");
    printf("  结果: 匹配\n\n");

    printf("  pattern: \"^hello$\"\n");
    printf("  text: \"hello\"\n");
    printf("  结果: 匹配\n\n");

    printf("  pattern: \"h.llo\"\n");
    printf("  text: \"hello\"\n");
    printf("  结果: 匹配\n");
}

// 演示 3: 通配符模式
static void demo_wildcard(void) {
    printf("\n=== 演示 3: 通配符模式 ===\n");

    printf("通配符匹配:\n\n");

    printf("* 匹配:\n");
    printf("  pattern: \"a*b\"\n");
    printf("  匹配: \"ab\", \"aab\", \"aaab\"\n");
    printf("  不匹配: \"acb\", \"ba\"\n\n");

    printf("? 匹配:\n");
    printf("  pattern: \"a?b\"\n");
    printf("  匹配: \"ab\", \"acb\"\n");
    printf("  不匹配: \"aab\", \"b\"\n\n");

    printf("字符类:\n");
    printf("  [abc] - 匹配 a, b, 或 c\n");
    printf("  [a-z] - 匹配任意小写字母\n");
    printf("  [0-9] - 匹配任意数字\n");
    printf("  [^abc] - 匹配非 a, b, c 的字符\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    printf("regex_config_t 结构:\n");
    printf("  case_sensitive: 是否区分大小写\n");
    printf("  multiline: 是否多行模式\n");
    printf("  dotall: 点号是否匹配换行\n");
    printf("  max_matches: 最大匹配次数\n");
    printf("  max_depth: 最大递归深度\n\n");

    printf("regex_default_config 函数:\n");
    printf("  功能: 获取默认配置\n");
    printf("  默认: 区分大小写，单行模式\n");
}

// 演示 5: 匹配结果
static void demo_result(void) {
    printf("\n=== 演示 5: 匹配结果 ===\n");

    printf("regex_match_result 函数:\n");
    printf("  功能: 匹配并返回详细信息\n");
    printf("  参数: pattern - 正则表达式\n");
    printf("        text - 文本\n");
    printf("        result - 结果结构\n");
    printf("  返回: 是否匹配\n\n");

    printf("regex_result_t 结构:\n");
    printf("  matched: 是否匹配\n");
    printf("  start: 匹配开始位置\n");
    printf("  end: 匹配结束位置\n");
    printf("  length: 匹配长度\n");
    printf("  groups: 捕获组数量\n");
    printf("  group_starts: 组开始位置\n");
    printf("  group_ends: 组结束位置\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  REGEX_OK - 成功\n");
    printf("  REGEX_ERROR_NULL_PTR - 空指针\n");
    printf("  REGEX_ERROR_INVALID_PATTERN - 无效模式\n");
    printf("  REGEX_ERROR_UNBALANCED_PAREN - 括号不匹配\n");
    printf("  REGEX_ERROR_UNBALANCED_BRACKET - 方括号不匹配\n");
    printf("  REGEX_ERROR_INVALID_ESCAPE - 无效转义\n");
    printf("  REGEX_ERROR_RECURSION_DEPTH - 递归深度超限\n");

    printf("\n注意事项:\n");
    printf("  - 检查模式语法\n");
    printf("  - 避免过度复杂模式\n");
    printf("  - 注意性能影响\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 输入验证\n");
    printf("   - 邮箱格式\n");
    printf("   - 电话号码\n");
    printf("   - 身份证号\n\n");

    printf("2. 文本搜索\n");
    printf("   - 日志分析\n");
    printf("   - 代码搜索\n");
    printf("   - 内容过滤\n\n");

    printf("3. 数据提取\n");
    printf("   - 解析配置文件\n");
    printf("   - 提取 URL\n");
    printf("   - 解析日期\n\n");

    printf("4. 文件名匹配\n");
    printf("   - 通配符匹配\n");
    printf("   - 批量处理\n");
    printf("   - 文件过滤\n\n");

    printf("5. 代码处理\n");
    printf("   - 语法高亮\n");
    printf("   - 代码格式化\n");
    printf("   - 静态分析\n");
}

int main(void) {
    printf("========================================\n");
    printf("    正则表达式演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_wildcard();
    demo_config();
    demo_result();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
