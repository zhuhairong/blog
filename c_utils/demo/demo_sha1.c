/**
 * SHA1 哈希演示程序
 *
 * 功能：
 * - 计算 SHA1 哈希值
 * - 文件哈希
 * - 字符串哈希
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../c_utils/sha1.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: SHA1 基本概念 ===\n");

    printf("SHA1 (Secure Hash Algorithm 1):\n\n");

    printf("特点:\n");
    printf("  - 输出 160 位 (20 字节) 哈希值\n");
    printf("  - 单向函数，不可逆\n");
    printf("  - 抗碰撞性（已弱化）\n\n");

    printf("注意:\n");
    printf("  - 已被证明不安全\n");
    printf("  - 不推荐用于安全场景\n");
    printf("  - 可用于非安全场景\n\n");

    printf("应用:\n");
    printf("  - 数据完整性检查\n");
    printf("  - 版本控制 (Git)\n");
    printf("  - 非安全校验\n");
}

// 演示 2: 基本哈希
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本哈希 ===\n");

    printf("sha1_init 函数:\n");
    printf("  功能: 初始化 SHA1 上下文\n");
    printf("  参数: ctx - 上下文指针\n\n");

    printf("sha1_update 函数:\n");
    printf("  功能: 更新哈希计算\n");
    printf("  参数: ctx - 上下文\n");
    printf("        data - 数据\n");
    printf("        len - 长度\n\n");

    printf("sha1_final 函数:\n");
    printf("  功能: 完成哈希计算\n");
    printf("  参数: ctx - 上下文\n");
    printf("        digest - 输出摘要 (20字节)\n");
}

// 演示 3: 一次性哈希
static void demo_oneshot(void) {
    printf("\n=== 演示 3: 一次性哈希 ===\n");

    printf("sha1_hash 函数:\n");
    printf("  功能: 一次性计算哈希\n");
    printf("  参数: data - 输入数据\n");
    printf("        len - 数据长度\n");
    printf("        digest - 输出摘要\n");
    printf("  返回: 错误码\n\n");

    printf("sha1_hash_string 函数:\n");
    printf("  功能: 计算字符串哈希\n");
    printf("  参数: str - 输入字符串\n");
    printf("        digest - 输出摘要\n\n");

    printf("sha1_hash_file 函数:\n");
    printf("  功能: 计算文件哈希\n");
    printf("  参数: filename - 文件名\n");
    printf("        digest - 输出摘要\n");
}

// 演示 4: 输出格式
static void demo_output(void) {
    printf("\n=== 演示 4: 输出格式 ===\n");

    printf("sha1_to_hex 函数:\n");
    printf("  功能: 转换为十六进制字符串\n");
    printf("  参数: digest - 摘要 (20字节)\n");
    printf("        hex_str - 输出字符串 (41字节)\n\n");

    printf("示例:\n");
    printf("  输入: \"hello\"\n");
    printf("  输出: 2ef7bde608ce5404e97d5f042f95f89f1c232871\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("sha1_config_t 结构:\n");
    printf("  enable_file_operations: 启用文件操作\n");
    printf("  enable_hex_output: 启用十六进制输出\n");
    printf("  enable_string_output: 启用字符串输出\n");
    printf("  max_input_size: 最大输入大小\n\n");

    printf("sha1_state_t 结构:\n");
    printf("  last_error: 最后错误\n");
    printf("  total_processed: 总处理字节数\n");
    printf("  is_initialized: 是否初始化\n");
    printf("  is_finalized: 是否已完成\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  SHA1_OK - 成功\n");
    printf("  SHA1_ERROR_INVALID_PARAMS - 无效参数\n");
    printf("  SHA1_ERROR_CONTEXT_NULL - 上下文为空\n");
    printf("  SHA1_ERROR_DATA_NULL - 数据为空\n");
    printf("  SHA1_ERROR_DIGEST_NULL - 摘要为空\n");
    printf("  SHA1_ERROR_BUFFER_TOO_SMALL - 缓冲区过小\n");
    printf("  SHA1_ERROR_FILE_OPEN - 文件打开失败\n");
    printf("  SHA1_ERROR_FILE_READ - 文件读取失败\n");

    printf("\n安全提示:\n");
    printf("  - 不要用于密码存储\n");
    printf("  - 不要用于数字签名\n");
    printf("  - 使用 SHA-256 或更高\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 数据完整性\n");
    printf("   - 文件校验\n");
    printf("   - 下载验证\n");
    printf("   - 备份校验\n\n");

    printf("2. 版本控制\n");
    printf("   - Git 对象标识\n");
    printf("   - 内容寻址\n");
    printf("   - 去重存储\n\n");

    printf("3. 数据去重\n");
    printf("   - 重复检测\n");
    printf("   - 块级去重\n");
    printf("   - 存储优化\n\n");

    printf("4. 缓存键\n");
    printf("   - 内容哈希键\n");
    printf("   - 快速查找\n");
    printf("   - 分布式缓存\n\n");

    printf("5. 非安全场景\n");
    printf("   - 快速校验\n");
    printf("   - 数据标识\n");
    printf("   - 日志追踪\n");
}

int main(void) {
    printf("========================================\n");
    printf("    SHA1 哈希演示\n");
    printf("========================================\n");
    printf("    警告: SHA1 已不安全!\n");
    printf("    仅用于非安全场景\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_oneshot();
    demo_output();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
