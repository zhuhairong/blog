/**
 * 重试策略演示程序
 *
 * 功能：
 * - 多种重试策略
 * - 指数退避
 * - 超时控制
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../c_utils/retry.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 重试策略基本概念 ===\n");

    printf("重试策略:\n\n");

    printf("目的:\n");
    printf("  - 处理临时性故障\n");
    printf("  - 提高系统可靠性\n");
    printf("  - 自动恢复机制\n\n");

    printf("延迟策略:\n");
    printf("  - CONSTANT: 固定延迟\n");
    printf("  - LINEAR: 线性增长\n");
    printf("  - EXPONENTIAL: 指数退避\n");
    printf("  - JITTER: 抖动延迟\n\n");

    printf("注意事项:\n");
    printf("  - 避免无限重试\n");
    printf("  - 设置最大延迟\n");
    printf("  - 区分可重试错误\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    printf("retry_init 函数:\n");
    printf("  功能: 初始化重试策略\n");
    printf("  参数: policy - 重试策略\n");
    printf("        max_attempts - 最大尝试次数\n");
    printf("        base_delay_ms - 基础延迟（毫秒）\n\n");

    printf("retry_execute 函数:\n");
    printf("  功能: 执行带重试的操作\n");
    printf("  参数: policy - 重试策略\n");
    printf("        operation - 操作函数\n");
    printf("        user_data - 用户数据\n");
    printf("        error - 错误信息\n");
    printf("  返回: 是否成功\n");
}

// 演示 3: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 3: 配置选项 ===\n");

    printf("retry_config_t 结构:\n");
    printf("  max_attempts: 最大尝试次数\n");
    printf("  base_delay_ms: 基础延迟（毫秒）\n");
    printf("  max_delay_ms: 最大延迟（毫秒）\n");
    printf("  max_total_time_ms: 最大总时间（毫秒）\n");
    printf("  delay_strategy: 延迟策略\n");
    printf("    - RETRY_DELAY_CONSTANT\n");
    printf("    - RETRY_DELAY_LINEAR\n");
    printf("    - RETRY_DELAY_EXPONENTIAL\n");
    printf("    - RETRY_DELAY_JITTER\n");
    printf("  jitter_factor: 抖动因子\n");
    printf("  backoff_factor: 退避因子\n");
    printf("  exponential_backoff: 是否使用指数退避\n\n");

    printf("retry_default_config 函数:\n");
    printf("  功能: 获取默认配置\n");
}

// 演示 4: 延迟策略对比
static void demo_strategies(void) {
    printf("\n=== 演示 4: 延迟策略对比 ===\n");

    int base_delay = 100;
    int max_delay = 5000;
    double backoff = 2.0;

    printf("基础延迟: %d ms, 最大延迟: %d ms\n\n", base_delay, max_delay);

    printf("恒定延迟:\n");
    for (int i = 1; i <= 5; i++) {
        printf("  尝试 %d: %d ms\n", i, base_delay);
    }

    printf("\n线性延迟:\n");
    for (int i = 1; i <= 5; i++) {
        int delay = base_delay * i;
        if (delay > max_delay) delay = max_delay;
        printf("  尝试 %d: %d ms\n", i, delay);
    }

    printf("\n指数退避 (退避因子 %.1f):\n", backoff);
    int delay = base_delay;
    for (int i = 1; i <= 5; i++) {
        if (delay > max_delay) delay = max_delay;
        printf("  尝试 %d: %d ms\n", i, delay);
        delay = (int)(delay * backoff);
    }
}

// 演示 5: 状态查询
static void demo_state(void) {
    printf("\n=== 演示 5: 状态查询 ===\n");

    printf("retry_get_state 函数:\n");
    printf("  功能: 获取重试策略当前状态\n");
    printf("  参数: policy - 重试策略\n");
    printf("        state - 状态结构输出\n");
    printf("        error - 错误信息\n");
    printf("  返回: 是否成功\n\n");

    printf("retry_state_t 结构:\n");
    printf("  current_attempt: 当前尝试次数\n");
    printf("  last_delay_ms: 上次延迟（毫秒）\n");
    printf("  next_delay_ms: 下次延迟（毫秒）\n");
    printf("  total_delay_ms: 总延迟（毫秒）\n");
    printf("  exhausted: 是否已耗尽尝试次数\n");
    printf("  timed_out: 是否已超时\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RETRY_OK - 成功\n");
    printf("  RETRY_ERROR_NULL_PTR - 空指针\n");
    printf("  RETRY_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RETRY_ERROR_MAX_ATTEMPTS_REACHED - 达到最大尝试\n");
    printf("  RETRY_ERROR_DELAY_TOO_LONG - 延迟过长\n");
    printf("  RETRY_ERROR_TIMEOUT - 超时\n");

    printf("\n注意事项:\n");
    printf("  - 区分可重试错误\n");
    printf("  - 记录重试日志\n");
    printf("  - 监控重试指标\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 网络请求\n");
    printf("   - HTTP 调用\n");
    printf("   - 数据库连接\n");
    printf("   - 消息队列\n\n");

    printf("2. 分布式系统\n");
    printf("   - 服务调用\n");
    printf("   - 配置获取\n");
    printf("   - 状态同步\n\n");

    printf("3. 文件操作\n");
    printf("   - 文件读取\n");
    printf("   - 网络存储\n");
    printf("   - 临时文件\n\n");

    printf("4. 外部服务\n");
    printf("   - 第三方 API\n");
    printf("   - 云服务\n");
    printf("   - 认证服务\n\n");

    printf("5. 资源获取\n");
    printf("   - 锁获取\n");
    printf("   - 连接池\n");
    printf("   - 限流等待\n");
}

int main(void) {
    printf("========================================\n");
    printf("    重试策略演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_config();
    demo_strategies();
    demo_state();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
