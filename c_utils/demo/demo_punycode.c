/**
 * Punycode 编码演示程序
 *
 * 功能：
 * - 国际化域名编码
 * - IDNA 兼容
 * - 编解码操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/punycode.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: Punycode 基本概念 ===\n");

    printf("Punycode:\n\n");

    printf("目的:\n");
    printf("  - 将 Unicode 域名编码为 ASCII\n");
    printf("  - 支持国际化域名 (IDN)\n");
    printf("  - 兼容现有 DNS 系统\n\n");

    printf("格式:\n");
    printf("  - 以 'xn--' 开头\n");
    printf("  - 后跟 Punycode 编码\n");
    printf("  - 示例: 中国 -> xn--fiqs8s\n\n");

    printf("应用场景:\n");
    printf("  - 中文域名\n");
    printf("  - 日文域名\n");
    printf("  - 阿拉伯文域名\n");
    printf("  - 其他非 ASCII 字符域名\n");
}

// 演示 2: 编码函数
static void demo_encode(void) {
    printf("\n=== 演示 2: Punycode 编码 ===\n");

    printf("punycode_encode 函数:\n");
    printf("  功能: 将 Unicode 码点数组编码为 Punycode\n");
    printf("  参数: input - Unicode 码点数组\n");
    printf("        input_len - 输入长度\n");
    printf("        output - 输出缓冲区\n");
    printf("        output_len - 输出长度(输入/输出)\n");
    printf("  返回: 是否成功\n\n");

    printf("示例用法:\n");
    printf("  uint32_t unicode[] = {20013, 22269};  // \"中国\"\n");
    printf("  char output[256];\n");
    printf("  size_t len = sizeof(output);\n");
    printf("  punycode_encode(unicode, 2, output, &len);\n");
    printf("  // 结果: \"fiqs8s\"\n");
}

// 演示 3: 解码函数
static void demo_decode(void) {
    printf("\n=== 演示 3: Punycode 解码 ===\n");

    printf("punycode_decode 函数:\n");
    printf("  功能: 将 Punycode 解码为 Unicode 码点数组\n");
    printf("  参数: input - Punycode 字符串\n");
    printf("        input_len - 输入长度\n");
    printf("        output - Unicode 码点数组\n");
    printf("        output_len - 输出长度(输入/输出)\n");
    printf("        error - 错误码输出\n");
    printf("  返回: 是否成功\n\n");

    printf("示例用法:\n");
    printf("  const char *punycode = \"fiqs8s\";\n");
    printf("  uint32_t unicode[256];\n");
    printf("  size_t len = 256;\n");
    printf("  punycode_error_t error;\n");
    printf("  punycode_decode(punycode, strlen(punycode),\n");
    printf("                  unicode, &len, &error);\n");
    printf("  // 结果: U+4E2D U+56FD (中国)\n");
}

// 演示 4: 域名编解码
static void demo_domain(void) {
    printf("\n=== 演示 4: 域名编解码 ===\n");

    printf("punycode_encode_domain 函数:\n");
    printf("  功能: 编码完整域名(IDNA 兼容)\n");
    printf("  参数: domain - 域名\n");
    printf("        output - 输出缓冲区\n");
    printf("        output_len - 输出长度\n");
    printf("        error - 错误码输出\n");
    printf("  返回: 是否成功\n\n");

    printf("punycode_decode_domain 函数:\n");
    printf("  功能: 解码完整域名\n");
    printf("  参数: domain - Punycode 编码域名\n");
    printf("        output - 输出缓冲区\n");
    printf("        output_len - 输出长度\n");
    printf("        error - 错误码输出\n");
    printf("  返回: 是否成功\n\n");

    printf("示例:\n");
    printf("  输入: \"例子.测试\"\n");
    printf("  输出: \"xn--fsq092h.xn--0zwm56d\"\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("punycode_config_t 结构:\n");
    printf("  strict: 严格验证输入\n");
    printf("  idna_compatible: 遵循 IDNA 标准\n");
    printf("  max_input_len: 限制输入大小\n");
    printf("  max_output_len: 限制输出大小\n\n");

    printf("配置说明:\n");
    printf("  - strict: 严格验证输入\n");
    printf("  - idna_compatible: 遵循 IDNA 标准\n");
    printf("  - max_input_len: 限制输入大小\n");
    printf("  - max_output_len: 限制输出大小\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  PUNYCODE_OK - 成功\n");
    printf("  PUNYCODE_ERROR_NULL_PTR - 空指针\n");
    printf("  PUNYCODE_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  PUNYCODE_ERROR_BUFFER_TOO_SMALL - 缓冲区太小\n");
    printf("  PUNYCODE_ERROR_INVALID_INPUT - 无效输入\n");
    printf("  PUNYCODE_ERROR_OVERFLOW - 溢出\n");

    printf("\n常见错误场景:\n");
    printf("  - 缓冲区不足 -> BUFFER_TOO_SMALL\n");
    printf("  - 无效 Unicode -> INVALID_INPUT\n");
    printf("  - 超长域名 -> OVERFLOW\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. Web 浏览器\n");
    printf("   - 显示国际化域名\n");
    printf("   - 地址栏转换\n");
    printf("   - 安全提示\n\n");

    printf("2. DNS 服务器\n");
    printf("   - 解析 IDN 域名\n");
    printf("   - 记录存储\n");
    printf("   - 查询处理\n\n");

    printf("3. 邮件系统\n");
    printf("   - 国际化邮箱地址\n");
    printf("   - 地址验证\n");
    printf("   - 邮件路由\n\n");

    printf("4. 网络安全\n");
    printf("   - 同源策略\n");
    printf("   - 钓鱼检测\n");
    printf("   - 证书验证\n\n");

    printf("5. 开发工具\n");
    printf("   - URL 处理\n");
    printf("   - 域名验证\n");
    printf("   - 国际化支持\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Punycode 编码演示\n");
    printf("========================================\n");

    demo_concept();
    demo_encode();
    demo_decode();
    demo_domain();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
