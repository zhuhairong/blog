/**
 * SHA256 精简版演示程序
 *
 * 功能：
 * - 计算 SHA256 哈希值
 * - 安全哈希算法
 * - 文件和字符串处理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../c_utils/sha256_tiny.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: SHA256 基本概念 ===\n");

    printf("SHA256 (Secure Hash Algorithm 256):\n\n");

    printf("特点:\n");
    printf("  - 输出 256 位 (32 字节) 哈希值\n");
    printf("  - SHA-2 家族成员\n");
    printf("  - 目前仍安全\n\n");

    printf("安全性:\n");
    printf("  - 抗碰撞攻击\n");
    printf("  - 抗原像攻击\n");
    printf("  - 推荐用于安全场景\n\n");

    printf("应用:\n");
    printf("  - 数字签名\n");
    printf("  - 证书验证\n");
    printf("  - 区块链\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    printf("sha256_tiny 函数:\n");
    printf("  功能: 计算 SHA256 哈希\n");
    printf("  参数: data - 输入数据\n");
    printf("        len - 数据长度\n");
    printf("        digest - 输出摘要 (32字节)\n\n");

    printf("示例:\n");
    printf("  输入: \"hello\"\n");
    printf("  输出: 2cf24dba5fb0a30e26e83b2ac5b9e29e...\n");
    printf("        (64字符十六进制)\n");
}

// 演示 3: 增强版函数
static void demo_enhanced(void) {
    printf("\n=== 演示 3: 增强版函数 ===\n");

    printf("sha256_tiny_ex 函数:\n");
    printf("  功能: 带配置的哈希计算\n");
    printf("  参数: data - 输入数据\n");
    printf("        len - 数据长度\n");
    printf("        digest - 输出摘要\n");
    printf("        config - 配置\n");
    printf("        state - 状态\n\n");

    printf("sha256_tiny_hash_string:\n");
    printf("  功能: 计算字符串哈希\n\n");

    printf("sha256_tiny_hash_file:\n");
    printf("  功能: 计算文件哈希\n");
}

// 演示 4: 输出格式
static void demo_output(void) {
    printf("\n=== 演示 4: 输出格式 ===\n");

    printf("sha256_tiny_to_hex 函数:\n");
    printf("  功能: 转换为十六进制字符串\n");
    printf("  参数: digest - 摘要 (32字节)\n");
    printf("        hex_str - 输出字符串 (65字节)\n\n");

    printf("sha256_tiny_compare 函数:\n");
    printf("  功能: 比较两个摘要\n");
    printf("  参数: digest1 - 第一个摘要\n");
    printf("        digest2 - 第二个摘要\n");
    printf("        result - 比较结果\n\n");

    printf("sha256_tiny_digest_size:\n");
    printf("  功能: 获取摘要大小\n");
    printf("  返回: 32 (字节)\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("sha256_tiny_config_t 结构:\n");
    printf("  enable_file_operations: 启用文件操作\n");
    printf("  enable_hex_output: 启用十六进制输出\n");
    printf("  enable_string_output: 启用字符串输出\n");
    printf("  max_input_size: 最大输入大小\n\n");

    printf("sha256_tiny_state_t 结构:\n");
    printf("  last_error: 最后错误\n");
    printf("  total_processed: 总处理字节数\n");
    printf("  is_initialized: 是否初始化\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  SHA256_TINY_OK - 成功\n");
    printf("  SHA256_TINY_ERROR_INVALID_PARAMS - 无效参数\n");
    printf("  SHA256_TINY_ERROR_DATA_NULL - 数据为空\n");
    printf("  SHA256_TINY_ERROR_DIGEST_NULL - 摘要为空\n");
    printf("  SHA256_TINY_ERROR_BUFFER_TOO_SMALL - 缓冲区过小\n");
    printf("  SHA256_TINY_ERROR_FILE_OPEN - 文件打开失败\n");
    printf("  SHA256_TINY_ERROR_FILE_READ - 文件读取失败\n");

    printf("\n安全建议:\n");
    printf("  - 使用常量时间比较\n");
    printf("  - 防止时序攻击\n");
    printf("  - 保护密钥材料\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 密码存储\n");
    printf("   - 哈希密码\n");
    printf("   - 加盐哈希\n");
    printf("   - 密钥派生\n\n");

    printf("2. 数字签名\n");
    printf("   - 消息摘要\n");
    printf("   - 签名验证\n");
    printf("   - 证书链\n\n");

    printf("3. 区块链技术\n");
    printf("   - 区块哈希\n");
    printf("   - 交易验证\n");
    printf("   - 工作量证明\n\n");

    printf("4. 数据完整性\n");
    printf("   - 安全校验\n");
    printf("   - 防篡改检测\n");
    printf("   - 软件分发\n\n");

    printf("5. 安全通信\n");
    printf("   - HMAC 计算\n");
    printf("   - 握手验证\n");
    printf("   - 密钥协商\n");
}

int main(void) {
    printf("========================================\n");
    printf("    SHA256 精简版演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_enhanced();
    demo_output();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
