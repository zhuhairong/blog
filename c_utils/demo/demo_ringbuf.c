/**
 * 环形缓冲区演示程序
 *
 * 功能：
 * - 循环数据存储
 * - 读写操作
 * - 状态查询
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/ringbuf.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 环形缓冲区基本概念 ===\n");

    printf("环形缓冲区:\n\n");

    printf("工作原理:\n");
    printf("  - 固定大小的缓冲区\n");
    printf("  - 读写指针循环移动\n");
    printf("  - 满了可以覆盖或阻塞\n\n");

    printf("优点:\n");
    printf("  - 无需数据搬移\n");
    printf("  - 内存效率高\n");
    printf("  - 适合流式数据\n\n");

    printf("应用:\n");
    printf("  - 串口通信\n");
    printf("  - 音频处理\n");
    printf("  - 日志缓冲\n");
}

// 演示 2: 创建和销毁
static void demo_creation(void) {
    printf("\n=== 演示 2: 创建和销毁 ===\n");

    printf("ringbuf_create 函数:\n");
    printf("  功能: 创建环形缓冲区\n");
    printf("  参数: capacity - 缓冲区容量\n");
    printf("  返回: 环形缓冲区指针\n\n");

    printf("ringbuf_create_ex 函数:\n");
    printf("  功能: 使用自定义配置创建\n");
    printf("  参数: config - 配置选项\n");
    printf("        error - 错误码输出\n\n");

    printf("ringbuf_free 函数:\n");
    printf("  功能: 销毁环形缓冲区\n");
    printf("  参数: rb - 环形缓冲区指针\n");
}

// 演示 3: 读写操作
static void demo_io(void) {
    printf("\n=== 演示 3: 读写操作 ===\n");

    printf("ringbuf_write 函数:\n");
    printf("  功能: 写入数据\n");
    printf("  参数: rb - 环形缓冲区\n");
    printf("        data - 数据指针\n");
    printf("        len - 数据长度\n");
    printf("  返回: 实际写入字节数\n\n");

    printf("ringbuf_read 函数:\n");
    printf("  功能: 读取数据\n");
    printf("  参数: rb - 环形缓冲区\n");
    printf("        data - 输出缓冲区\n");
    printf("        len - 读取长度\n");
    printf("  返回: 实际读取字节数\n\n");

    printf("ringbuf_peek 函数:\n");
    printf("  功能: 预览数据（不移动读指针）\n");
}

// 演示 4: 状态查询
static void demo_state(void) {
    printf("\n=== 演示 4: 状态查询 ===\n");

    printf("ringbuf_state_t 结构:\n");
    printf("  capacity: 总容量\n");
    printf("  size: 当前使用大小\n");
    printf("  available: 可用空间\n");
    printf("  read_pos: 读取位置\n");
    printf("  write_pos: 写入位置\n");
    printf("  is_full: 是否已满\n");
    printf("  is_empty: 是否为空\n\n");

    printf("ringbuf_get_state 函数:\n");
    printf("  功能: 获取当前状态\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("ringbuf_config_t 结构:\n");
    printf("  capacity: 缓冲区容量\n");
    printf("  zero_initialize: 是否零初始化\n");
    printf("  thread_safe: 是否线程安全\n");
    printf("  overwrite: 是否覆盖模式\n");
    printf("  alignment: 内存对齐要求\n\n");

    printf("覆盖模式:\n");
    printf("  - true: 满了覆盖旧数据\n");
    printf("  - false: 满了返回错误\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RINGBUF_OK - 成功\n");
    printf("  RINGBUF_ERROR_NULL_PTR - 空指针\n");
    printf("  RINGBUF_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RINGBUF_ERROR_CAPACITY_TOO_SMALL - 容量过小\n");
    printf("  RINGBUF_ERROR_OUT_OF_MEMORY - 内存不足\n");
    printf("  RINGBUF_ERROR_BUFFER_FULL - 缓冲区已满\n");
    printf("  RINGBUF_ERROR_BUFFER_EMPTY - 缓冲区为空\n");

    printf("\n注意事项:\n");
    printf("  - 检查返回值\n");
    printf("  - 处理满/空情况\n");
    printf("  - 注意线程安全\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 串口通信\n");
    printf("   - 接收缓冲区\n");
    printf("   - 发送缓冲区\n");
    printf("   - 流控处理\n\n");

    printf("2. 音频处理\n");
    printf("   - 音频缓冲\n");
    printf("   - 实时播放\n");
    printf("   - 延迟控制\n\n");

    printf("3. 日志系统\n");
    printf("   - 异步日志\n");
    printf("   - 批量写入\n");
    printf("   - 性能优化\n\n");

    printf("4. 网络编程\n");
    printf("   - 数据包缓冲\n");
    printf("   - 流量控制\n");
    printf("   - 协议解析\n\n");

    printf("5. 生产者-消费者\n");
    printf("   - 解耦生产消费\n");
    printf("   - 速率匹配\n");
    printf("   - 缓冲峰值\n");
}

int main(void) {
    printf("========================================\n");
    printf("    环形缓冲区演示\n");
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
