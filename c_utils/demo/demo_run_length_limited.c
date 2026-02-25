/**
 * 游程长度受限编码演示程序
 *
 * 功能：
 * - RLL 编码解码
 * - 磁盘存储优化
 * - 多种编码方案
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/run_length_limited.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: RLL 游程长度受限基本概念 ===\n");

    printf("RLL (Run-Length Limited):\n\n");

    printf("原理:\n");
    printf("  - 限制连续 0 或 1 的最小/最大长度\n");
    printf("  - 用于磁盘、磁带存储\n");
    printf("  - 保证时钟同步\n\n");

    printf("RLL(d,k) 表示:\n");
    printf("  d: 最小连续长度\n");
    printf("  k: 最大连续长度\n\n");

    printf("常见方案:\n");
    printf("  RLL(1,7): 软盘、硬盘\n");
    printf("  RLL(2,7): 高密度存储\n");
}

// 演示 2: 基本编码
static void demo_encode(void) {
    printf("\n=== 演示 2: 基本编码 ===\n");

    printf("rll_encode 函数:\n");
    printf("  功能: RLL 编码\n");
    printf("  参数: in - 输入数据\n");
    printf("        in_len - 输入长度\n");
    printf("        out - 输出缓冲区\n");
    printf("        out_size - 输出大小\n");
    printf("        config - 配置\n");
    printf("        result - 结果\n");
    printf("        error - 错误码\n");
    printf("  返回: 编码后长度\n\n");

    printf("rll_encode_basic 函数:\n");
    printf("  功能: 基础编码示例\n");
}

// 演示 3: 解码操作
static void demo_decode(void) {
    printf("\n=== 演示 3: 解码操作 ===\n");

    printf("rll_decode 函数:\n");
    printf("  功能: RLL 解码\n");
    printf("  参数: in - 输入数据\n");
    printf("        in_len - 输入长度\n");
    printf("        out - 输出缓冲区\n");
    printf("        out_size - 输出大小\n");
    printf("        config - 配置\n");
    printf("        result - 结果\n");
    printf("        error - 错误码\n");
    printf("  返回: 解码后长度\n");
}

// 演示 4: RLL(1,7) 编码
static void demo_rll17(void) {
    printf("\n=== 演示 4: RLL(1,7) 编码 ===\n");

    printf("rll_1_7_encode 函数:\n");
    printf("  功能: RLL(1,7) 编码\n");
    printf("  特点:\n");
    printf("    - 最小 1 个连续\n");
    printf("    - 最大 7 个连续\n");
    printf("    - 编码率: 2/3\n\n");

    printf("rll_1_7_decode 函数:\n");
    printf("  功能: RLL(1,7) 解码\n\n");

    printf("应用:\n");
    printf("  - 软盘驱动器\n");
    printf("  - 早期硬盘\n");
    printf("  - 磁带存储\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("rll_config_t 结构:\n");
    printf("  min_run_length: 最小运行长度\n");
    printf("  max_run_length: 最大运行长度\n");
    printf("  use_optimized: 是否使用优化\n");
    printf("  check_input: 是否检查输入\n");
    printf("  check_output: 是否检查输出\n");
    printf("  max_input_size: 最大输入大小\n");
    printf("  max_output_size: 最大输出大小\n");
    printf("  use_variable_length: 是否使用可变长度\n\n");

    printf("rll_default_config:\n");
    printf("  获取默认配置\n\n");

    printf("rll_1_7_default_config:\n");
    printf("  获取 RLL(1,7) 配置\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RLL_OK - 成功\n");
    printf("  RLL_ERROR_NULL_PTR - 空指针\n");
    printf("  RLL_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RLL_ERROR_INPUT_TOO_LARGE - 输入过大\n");
    printf("  RLL_ERROR_OUTPUT_TOO_SMALL - 输出过小\n");
    printf("  RLL_ERROR_INVALID_RUN_LENGTH - 无效运行长度\n");
    printf("  RLL_ERROR_ENCODING_FAILED - 编码失败\n");
    printf("  RLL_ERROR_DECODING_FAILED - 解码失败\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 磁盘存储\n");
    printf("   - 硬盘驱动器\n");
    printf("   - 软盘驱动器\n");
    printf("   - 光盘存储\n\n");

    printf("2. 磁带存储\n");
    printf("   - 数据备份\n");
    printf("   - 归档存储\n");
    printf("   - 流媒体\n\n");

    printf("3. 通信系统\n");
    printf("   - 时钟恢复\n");
    printf("   - 信号同步\n");
    printf("   - 数据传输\n\n");

    printf("4. 存储优化\n");
    printf("   - 提高存储密度\n");
    printf("   - 改善信号质量\n");
    printf("   - 降低误码率\n\n");

    printf("5. 历史研究\n");
    printf("   - 老式存储设备\n");
    printf("   - 数据恢复\n");
    printf("   - 技术考古\n");
}

int main(void) {
    printf("========================================\n");
    printf("    RLL 游程长度受限编码演示\n");
    printf("========================================\n");

    demo_concept();
    demo_encode();
    demo_decode();
    demo_rll17();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
