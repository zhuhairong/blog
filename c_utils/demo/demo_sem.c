/**
 * 信号量演示程序
 *
 * 功能：
 * - 进程/线程同步
 * - 资源计数
 * - 互斥访问
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/sem.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 信号量基本概念 ===\n");

    printf("信号量:\n\n");

    printf("定义:\n");
    printf("  - 用于进程/线程同步的计数器\n");
    printf("  - 支持 P(等待) 和 V(信号) 操作\n");
    printf("  - 可命名或匿名\n\n");

    printf("类型:\n");
    printf("  - 二进制信号量: 0 或 1\n");
    printf("  - 计数信号量: 0 到 N\n\n");

    printf("操作:\n");
    printf("  P(wait): 等待，计数减 1\n");
    printf("  V(post): 信号，计数加 1\n");
}

// 演示 2: 创建和销毁
static void demo_creation(void) {
    printf("\n=== 演示 2: 创建和销毁 ===\n");

    printf("sem_create 函数:\n");
    printf("  功能: 创建信号量\n");
    printf("  参数: name - 名称(NULL为匿名)\n");
    printf("        value - 初始值\n");
    printf("  返回: 信号量句柄\n\n");

    printf("sem_create_ex 函数:\n");
    printf("  功能: 增强版创建\n");
    printf("  参数: config - 配置选项\n");
    printf("        state - 状态输出\n\n");

    printf("sem_close_delete 函数:\n");
    printf("  功能: 关闭并删除信号量\n");
}

// 演示 3: P/V 操作
static void demo_pv(void) {
    printf("\n=== 演示 3: P/V 操作 ===\n");

    printf("sem_p 函数 (P操作/等待):\n");
    printf("  功能: 等待信号量\n");
    printf("  行为: 计数 > 0 时减 1 继续\n");
    printf("        计数 = 0 时阻塞等待\n\n");

    printf("sem_v 函数 (V操作/信号):\n");
    printf("  功能: 释放信号量\n");
    printf("  行为: 计数加 1\n");
    printf("        唤醒等待的进程\n\n");

    printf("sem_trywait 函数:\n");
    printf("  功能: 非阻塞尝试等待\n");
    printf("  返回: 成功或立即返回错误\n");
}

// 演示 4: 超时等待
static void demo_timed(void) {
    printf("\n=== 演示 4: 超时等待 ===\n");

    printf("sem_timedwait 函数:\n");
    printf("  功能: 带超时的等待\n");
    printf("  参数: sem - 信号量\n");
    printf("        timeout_ms - 超时毫秒\n");
    printf("        state - 状态输出\n\n");

    printf("返回值:\n");
    printf("  SEM_OK: 成功获取\n");
    printf("  SEM_ERROR_TIMEDWAIT: 超时\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("sem_config_t 结构:\n");
    printf("  create_if_not_exists: 不存在则创建\n");
    printf("  exclusive: 独占模式\n");
    printf("  unlink_on_close: 关闭时删除\n");
    printf("  max_value: 最大值(0无限制)\n\n");

    printf("sem_state_t 结构:\n");
    printf("  last_error: 最后错误\n");
    printf("  error_code: 系统错误码\n");
    printf("  current_value: 当前值\n");
    printf("  is_initialized: 是否初始化\n");
    printf("  is_named: 是否命名信号量\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  SEM_OK - 成功\n");
    printf("  SEM_ERROR_INVALID_PARAMS - 无效参数\n");
    printf("  SEM_ERROR_CREATE - 创建失败\n");
    printf("  SEM_ERROR_OPEN - 打开失败\n");
    printf("  SEM_ERROR_WAIT - 等待失败\n");
    printf("  SEM_ERROR_POST - 信号失败\n");
    printf("  SEM_ERROR_TRYWAIT - 尝试等待失败\n");
    printf("  SEM_ERROR_TIMEDWAIT - 超时\n");

    printf("\n注意事项:\n");
    printf("  - 检查返回值\n");
    printf("  - 正确处理超时\n");
    printf("  - 避免死锁\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 互斥访问\n");
    printf("   - 临界区保护\n");
    printf("   - 资源独占\n");
    printf("   - 二进制信号量\n\n");

    printf("2. 资源池\n");
    printf("   - 连接池\n");
    printf("   - 线程池\n");
    printf("   - 对象池\n\n");

    printf("3. 生产者-消费者\n");
    printf("   - 缓冲区同步\n");
    printf("   - 计数信号量\n");
    printf("   - 流量控制\n\n");

    printf("4. 进程同步\n");
    printf("   - 命名信号量\n");
    printf("   - 跨进程通信\n");
    printf("   - 协调执行\n\n");

    printf("5. 事件通知\n");
    printf("   - 任务完成通知\n");
    printf("   - 状态变更\n");
    printf("   - 唤醒等待\n");
}

int main(void) {
    printf("========================================\n");
    printf("    信号量演示\n");
    printf("========================================\n");

    demo_concept();
    demo_creation();
    demo_pv();
    demo_timed();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
