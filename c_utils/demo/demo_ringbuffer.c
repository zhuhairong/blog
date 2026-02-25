/**
 * 环形缓冲区(增强版)演示程序
 *
 * 功能：
 * - 增强的环形缓冲区
 * - 更多配置选项
 * - 线程安全支持
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/ringbuffer.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 增强版环形缓冲区基本概念 ===\n");

    printf("增强版环形缓冲区:\n\n");

    printf("与 ringbuf 的区别:\n");
    printf("  - 更多配置选项\n");
    printf("  - 更灵活的使用方式\n");
    printf("  - 增强的错误处理\n\n");

    printf("特性:\n");
    printf("  - 可选内存分配方式\n");
    printf("  - 更详细的状态信息\n");
    printf("  - 更好的线程安全支持\n");
}

// 演示 2: 创建和销毁
static void demo_creation(void) {
    printf("\n=== 演示 2: 创建和销毁 ===\n");

    printf("ringbuffer_create 函数:\n");
    printf("  功能: 创建环形缓冲区\n");
    printf("  参数: size - 缓冲区大小\n");
    printf("  返回: 环形缓冲区指针\n\n");

    printf("ringbuffer_create_ex 函数:\n");
    printf("  功能: 使用自定义配置创建\n");
    printf("  参数: config - 配置选项\n");
    printf("        error - 错误码输出\n\n");

    printf("ringbuffer_free 函数:\n");
    printf("  功能: 销毁环形缓冲区\n");
}

// 演示 3: 读写操作
static void demo_io(void) {
    printf("\n=== 演示 3: 读写操作 ===\n");

    printf("ringbuffer_write 函数:\n");
    printf("  功能: 写入数据\n");
    printf("  参数: rb - 环形缓冲区\n");
    printf("        data - 数据指针\n");
    printf("        len - 数据长度\n");
    printf("  返回: 实际写入字节数\n\n");

    printf("ringbuffer_read 函数:\n");
    printf("  功能: 读取数据\n");
    printf("  参数: rb - 环形缓冲区\n");
    printf("        data - 输出缓冲区\n");
    printf("        len - 读取长度\n");
    printf("  返回: 实际读取字节数\n\n");

    printf("ringbuffer_skip 函数:\n");
    printf("  功能: 跳过指定字节\n");
}

// 演示 4: 状态查询
static void demo_state(void) {
    printf("\n=== 演示 4: 状态查询 ===\n");

    printf("ringbuffer_state_t 结构:\n");
    printf("  size: 总大小\n");
    printf("  used: 已使用大小\n");
    printf("  available_read: 可读取大小\n");
    printf("  available_write: 可写入大小\n");
    printf("  read_pos: 读取位置\n");
    printf("  write_pos: 写入位置\n");
    printf("  is_full: 是否已满\n");
    printf("  is_empty: 是否为空\n\n");

    printf("ringbuffer_get_state 函数:\n");
    printf("  功能: 获取当前状态\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("ringbuffer_config_t 结构:\n");
    printf("  size: 缓冲区大小\n");
    printf("  zero_initialize: 是否零初始化\n");
    printf("  thread_safe: 是否线程安全\n");
    printf("  overwrite: 是否覆盖模式\n");
    printf("  use_malloc: 是否使用 malloc\n");
    printf("  alignment: 内存对齐要求\n\n");

    printf("use_malloc 选项:\n");
    printf("  - true: 使用 malloc 动态分配\n");
    printf("  - false: 使用静态内存或用户提供\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RINGBUFFER_OK - 成功\n");
    printf("  RINGBUFFER_ERROR_NULL_PTR - 空指针\n");
    printf("  RINGBUFFER_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RINGBUFFER_ERROR_SIZE_TOO_SMALL - 大小过小\n");
    printf("  RINGBUFFER_ERROR_OUT_OF_MEMORY - 内存不足\n");
    printf("  RINGBUFFER_ERROR_FULL - 缓冲区已满\n");
    printf("  RINGBUFFER_ERROR_EMPTY - 缓冲区为空\n");

    printf("\n错误处理建议:\n");
    printf("  - 始终检查错误码\n");
    printf("  - 使用增强版函数获取详细信息\n");
    printf("  - 合理配置缓冲区大小\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 嵌入式系统\n");
    printf("   - 静态内存分配\n");
    printf("   - 资源受限环境\n");
    printf("   - 确定性行为\n\n");

    printf("2. 高性能服务器\n");
    printf("   - 零拷贝优化\n");
    printf("   - 线程安全访问\n");
    printf("   - 高并发处理\n\n");

    printf("3. 实时系统\n");
    printf("   - 确定性延迟\n");
    printf("   - 无动态分配\n");
    printf("   - 可预测性能\n\n");

    printf("4. 数据传输\n");
    printf("   - 流式数据处理\n");
    printf("   - 缓冲和整形\n");
    printf("   - 速率控制\n\n");

    printf("5. 多媒体处理\n");
    printf("   - 音视频缓冲\n");
    printf("   - 同步处理\n");
    printf("   - 平滑播放\n");
}

int main(void) {
    printf("========================================\n");
    printf("    增强版环形缓冲区演示\n");
    printf("========================================\n");

    demo_concept();
    demo_creation();
    demo_io();
    demo_state();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
