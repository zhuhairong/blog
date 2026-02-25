/**
 * RLE 游程编码演示程序
 *
 * 功能：
 * - 简单的无损压缩
 * - 适合重复数据
 * - 编码解码操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/rle.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: RLE 游程编码基本概念 ===\n");

    printf("RLE (Run-Length Encoding):\n\n");

    printf("原理:\n");
    printf("  - 将连续重复的数据存储为 (计数, 值) 对\n");
    printf("  - 适合有大量连续重复数据的场景\n\n");

    printf("示例:\n");
    printf("  原始: AAAABBBCCDAA\n");
    printf("  编码: 4A3B2C1D2A\n");
    printf("  压缩比: 12 -> 10 (83.3%%)\n\n");

    printf("适用场景:\n");
    printf("  - 简单图形\n");
    printf("  - 传真图像\n");
    printf("  - 某些文本数据\n");
}

// 演示 2: 基本压缩
static void demo_encode(void) {
    printf("\n=== 演示 2: 基本压缩 ===\n");

    printf("rle_encode 函数:\n");
    printf("  功能: RLE 压缩\n");
    printf("  参数: in - 输入数据\n");
    printf("        in_len - 输入长度\n");
    printf("        out - 输出缓冲区\n");
    printf("  返回: 压缩后长度\n\n");

    printf("示例:\n");
    printf("  输入: \"WWWWAAADEXXXXXX\"\n");
    printf("  输出: \"4W3A1D1E6X\"\n");
    printf("  压缩率: 15 -> 10 (66.7%%)\n");
}

// 演示 3: 解压缩
static void demo_decode(void) {
    printf("\n=== 演示 3: 解压缩 ===\n");

    printf("rle_decode 函数:\n");
    printf("  功能: RLE 解压缩\n");
    printf("  参数: in - 输入数据\n");
    printf("        in_len - 输入长度\n");
    printf("        out - 输出缓冲区\n");
    printf("  返回: 解压缩后长度\n\n");

    printf("示例:\n");
    printf("  输入: \"4W3A1D1E6X\"\n");
    printf("  输出: \"WWWWAAADEXXXXXX\"\n");
    printf("  还原成功!\n");
}

// 演示 4: 增强版函数
static void demo_enhanced(void) {
    printf("\n=== 演示 4: 增强版函数 ===\n");

    printf("rle_encode_ex 函数:\n");
    printf("  功能: 带配置的 RLE 压缩\n");
    printf("  额外参数:\n");
    printf("    out_size - 输出缓冲区大小\n");
    printf("    config - 配置选项\n");
    printf("    result - 压缩结果\n");
    printf("    error - 错误码\n\n");

    printf("rle_result_t 结构:\n");
    printf("  input_size: 输入大小\n");
    printf("  output_size: 输出大小\n");
    printf("  run_count: 运行次数\n");
    printf("  compression_ratio: 压缩比\n");
    printf("  compressed: 是否成功压缩\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("rle_config_t 结构:\n");
    printf("  use_optimized: 是否使用优化模式\n");
    printf("  use_variable_length: 是否使用可变长度计数\n");
    printf("  check_input: 是否检查输入数据\n");
    printf("  check_output: 是否检查输出空间\n");
    printf("  max_run_length: 最大运行长度\n");
    printf("  min_run_length: 最小运行长度\n");
    printf("  max_input_size: 最大输入大小\n");
    printf("  max_output_size: 最大输出大小\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RLE_OK - 成功\n");
    printf("  RLE_ERROR_NULL_PTR - 空指针\n");
    printf("  RLE_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RLE_ERROR_INPUT_TOO_LARGE - 输入过大\n");
    printf("  RLE_ERROR_OUTPUT_TOO_SMALL - 输出过小\n");
    printf("  RLE_ERROR_INVALID_ENCODED_DATA - 无效编码数据\n");
    printf("  RLE_ERROR_COMPRESSION_FAILED - 压缩失败\n");
    printf("  RLE_ERROR_DECOMPRESSION_FAILED - 解压缩失败\n");

    printf("\n注意事项:\n");
    printf("  - 确保输出缓冲区足够大\n");
    printf("  - 检查输入数据有效性\n");
    printf("  - 处理边界情况\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 图像压缩\n");
    printf("   - 传真图像 (TIFF)\n");
    printf("   - 简单图形\n");
    printf("   - 图标文件\n\n");

    printf("2. 文本处理\n");
    printf("   - 重复字符多的文本\n");
    printf("   - 日志文件\n");
    printf("   - 配置文件\n\n");

    printf("3. 游戏资源\n");
    printf("   - 地图数据\n");
    printf("   - 精灵图\n");
    printf("   - 关卡数据\n\n");

    printf("4. 数据存储\n");
    printf("   - 备份数据\n");
    printf("   - 归档文件\n");
    printf("   - 传输优化\n\n");

    printf("5. 与其他算法结合\n");
    printf("   - RLE + Huffman\n");
    printf("   - RLE + LZ77\n");
    printf("   - 多级压缩\n");
}

int main(void) {
    printf("========================================\n");
    printf("    RLE 游程编码演示\n");
    printf("========================================\n");

    demo_concept();
    demo_encode();
    demo_decode();
    demo_enhanced();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
