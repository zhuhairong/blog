/**
 * HTML 编解码演示程序
 *
 * 功能：
 * - HTML 实体编码
 * - HTML 实体解码
 * - 特殊字符处理
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/html_codec.h"

// 演示 1: 基本编码
static void demo_encode(void) {
    printf("\n=== 演示 1: HTML 编码 ===\n");

    html_codec_error_t error;

    struct {
        const char *input;
        const char *description;
    } cases[] = {
        {"<div>Hello</div>", "HTML 标签"},
        {"5 > 3 && 3 < 5", "比较运算符"},
        {"Tom & Jerry", "& 符号"},
        {"\"quoted\" text", "引号"},
        {"It's working", "撇号"},
        {NULL, NULL}
    };

    printf("HTML 编码示例:\n\n");

    char output[256];
    for (int i = 0; cases[i].input; i++) {
        size_t len = html_encode(cases[i].input, output, sizeof(output),
                                  HTML_CODEC_FLAG_NONE, &error);
        if (len > 0) {
            printf("%s:\n", cases[i].description);
            printf("  输入:  %s\n", cases[i].input);
            printf("  输出:  %s\n\n", output);
        }
    }
}

// 演示 2: 基本解码
static void demo_decode(void) {
    printf("\n=== 演示 2: HTML 解码 ===\n");

    html_codec_error_t error;

    struct {
        const char *input;
        const char *description;
    } cases[] = {
        {"&lt;div&gt;Hello&lt;/div&gt;", "HTML 标签"},
        {"5 &gt; 3 &amp;&amp; 3 &lt; 5", "比较运算符"},
        {"Tom &amp; Jerry", "& 符号"},
        {"&quot;quoted&quot; text", "引号"},
        {"It&apos;s working", "撇号"},
        {"&#169; 2024 Company", "数字实体 (版权)"},
        {"&#x263A; Smile", "十六进制实体"},
        {NULL, NULL}
    };

    printf("HTML 解码示例:\n\n");

    char output[256];
    for (int i = 0; cases[i].input; i++) {
        size_t len = html_decode(cases[i].input, output, sizeof(output),
                                  HTML_CODEC_FLAG_NONE, &error);
        if (len > 0) {
            printf("%s:\n", cases[i].description);
            printf("  输入:  %s\n", cases[i].input);
            printf("  输出:  %s\n\n", output);
        }
    }
}

// 演示 3: 编码选项
static void demo_encode_options(void) {
    printf("\n=== 演示 3: 编码选项 ===\n");

    html_codec_error_t error;
    const char *input = "<tag attr=\"value\">Tom & Jerry's</tag>";

    printf("输入: %s\n\n", input);

    char output[256];
    size_t len;

    // 默认编码
    len = html_encode(input, output, sizeof(output), HTML_CODEC_FLAG_NONE, &error);
    printf("默认配置:\n  %s\n\n", output);

    // 编码所有字符
    len = html_encode(input, output, sizeof(output), HTML_CODEC_FLAG_ENCODE_ALL, &error);
    printf("编码所有:\n  %s\n\n", output);

    // 只编码非 ASCII
    len = html_encode(input, output, sizeof(output), HTML_CODEC_FLAG_ENCODE_NON_ASCII, &error);
    printf("只编码非 ASCII:\n  %s\n", output);
}

// 演示 4: 常用 HTML 实体
static void demo_entities(void) {
    printf("\n=== 演示 4: 常用 HTML 实体 ===\n");

    printf("基本实体:\n");
    printf("  &amp;   -> &   (和号)\n");
    printf("  &lt;    -> <   (小于)\n");
    printf("  &gt;    -> >   (大于)\n");
    printf("  &quot;  -> \"   (引号)\n");
    printf("  &apos;  -> '   (撇号)\n");
    printf("\n");

    printf("常用符号:\n");
    printf("  &copy;  -> ©   (版权)\n");
    printf("  &reg;   -> ®   (注册商标)\n");
    printf("  &trade; -> ™   (商标)\n");
    printf("  &nbsp;  ->     (不间断空格)\n");
    printf("\n");

    printf("数学符号:\n");
    printf("  &times; -> ×   (乘号)\n");
    printf("  &divide;-> ÷   (除号)\n");
    printf("  &plusmn;-> ±   (加减)\n");
    printf("  &deg;   -> °   (度)\n");
}

// 演示 5: XSS 防护
static void demo_xss_protection(void) {
    printf("\n=== 演示 5: XSS 防护 ===\n");

    html_codec_error_t error;

    printf("XSS 攻击示例:\n");

    const char *attacks[] = {
        "<script>alert('XSS')</script>",
        "<img src=x onerror=alert('XSS')>",
        "<body onload=alert('XSS')>",
        "javascript:alert('XSS')",
        NULL
    };

    char output[512];
    for (int i = 0; attacks[i]; i++) {
        printf("\n攻击载荷 %d:\n", i + 1);
        printf("  原始: %s\n", attacks[i]);

        html_encode(attacks[i], output, sizeof(output),
                    HTML_CODEC_FLAG_NONE, &error);
        printf("  编码: %s\n", output);
        printf("  结果: 安全显示，不会执行\n");
    }

    printf("\n重要: 始终对不可信输入进行 HTML 编码!\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. Web 开发\n");
    printf("   - 防止 XSS 攻击\n");
    printf("   - 显示用户输入\n");
    printf("   - 富文本编辑\n");
    printf("\n");

    printf("2. 邮件处理\n");
    printf("   - HTML 邮件编码\n");
    printf("   - 特殊字符处理\n");
    printf("\n");

    printf("3. 数据导出\n");
    printf("   - CSV/Excel 导出\n");
    printf("   - XML/HTML 生成\n");
    printf("\n");

    printf("4. 日志记录\n");
    printf("   - 安全记录用户输入\n");
    printf("   - 防止日志注入\n");
}

int main(void) {
    printf("========================================\n");
    printf("    HTML 编解码演示\n");
    printf("========================================\n");

    demo_encode();
    demo_decode();
    demo_encode_options();
    demo_entities();
    demo_xss_protection();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
