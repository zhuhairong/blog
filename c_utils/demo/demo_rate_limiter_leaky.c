/**
 * 漏桶限流器演示程序
 *
 * 功能：
 * - 流量控制
 * - 平滑突发流量
 * - 多种策略
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../c_utils/rate_limiter_leaky.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 漏桶限流器基本概念 ===\n");

    printf("漏桶算法:\n\n");

    printf("工作原理:\n");
    printf("  - 请求像水一样流入桶中\n");
    printf("  - 桶以固定速率漏水\n");
    printf("  - 桶满时拒绝新请求\n\n");

    printf("特点:\n");
    printf("  - 平滑突发流量\n");
    printf("  - 固定输出速率\n");
    printf("  - 适合下游处理能力有限\n\n");

    printf("与令牌桶对比:\n");
    printf("  漏桶: 固定速率输出，平滑流量\n");
    printf("  令牌桶: 允许突发，平均速率限制\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    printf("leaky_init 函数:\n");
    printf("  功能: 初始化漏桶限流器\n");
    printf("  参数: lb - 漏桶限流器\n");
    printf("        capacity - 桶容量\n");
    printf("        rate - 漏水速率（单位/秒）\n\n");

    printf("leaky_consume 函数:\n");
    printf("  功能: 尝试消耗指定数量的令牌\n");
    printf("  参数: lb - 漏桶限流器\n");
    printf("        amount - 消耗数量\n");
    printf("  返回: 是否成功\n");
}

// 演示 3: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 3: 配置选项 ===\n");

    printf("leaky_bucket_config_t 结构:\n");
    printf("  capacity: 桶容量\n");
    printf("  rate: 漏水速率（单位/秒）\n");
    printf("  initial_water: 初始水量\n");
    printf("  use_monotonic_time: 是否使用单调时间\n");
    printf("  allow_burst: 是否允许突发流量\n\n");

    printf("leaky_bucket_t 结构:\n");
    printf("  capacity: 桶容量\n");
    printf("  rate: 漏水速率\n");
    printf("  water: 当前水量\n");
    printf("  last_time: 最后更新时间\n");
    printf("  config: 配置\n");
}

// 演示 4: 状态查询
static void demo_state(void) {
    printf("\n=== 演示 4: 状态查询 ===\n");

    printf("leaky_bucket_state_t 结构:\n");
    printf("  current_water: 当前水量\n");
    printf("  available_capacity: 可用容量\n");
    printf("  fill_level: 填充水平 (0.0-1.0)\n");
    printf("  last_update_time: 最后更新时间\n");
    printf("  is_full: 是否已满\n");
    printf("  is_empty: 是否为空\n\n");

    printf("leaky_get_state 函数:\n");
    printf("  功能: 获取漏桶当前状态\n");
}

// 演示 5: 等待消费
static void demo_wait(void) {
    printf("\n=== 演示 5: 等待消费 ===\n");

    printf("leaky_consume_wait 函数:\n");
    printf("  功能: 尝试消耗，如不足则等待\n");
    printf("  参数: lb - 漏桶限流器\n");
    printf("        amount - 消耗数量\n");
    printf("        max_wait_ms - 最大等待时间（毫秒）\n");
    printf("        actual_wait_ms - 实际等待时间输出\n");
    printf("  返回: 是否成功\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  LEAKY_BUCKET_OK - 成功\n");
    printf("  LEAKY_BUCKET_ERROR_NULL_PTR - 空指针\n");
    printf("  LEAKY_BUCKET_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL - 容量过小\n");
    printf("  LEAKY_BUCKET_ERROR_RATE_TOO_SLOW - 速率过慢\n");
    printf("  LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE - 消耗数量过大\n");

    printf("\n注意事项:\n");
    printf("  - 容量必须大于 0\n");
    printf("  - 速率必须为正\n");
    printf("  - 消耗量不能超过容量\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. API 限流\n");
    printf("   - 保护后端服务\n");
    printf("   - 防止过载\n");
    printf("   - 公平分配资源\n\n");

    printf("2. 网络流量控制\n");
    printf("   - 带宽限制\n");
    printf("   - QoS 控制\n");
    printf("   - 流量整形\n\n");

    printf("3. 数据库访问\n");
    printf("   - 连接池限流\n");
    printf("   - 查询限流\n");
    printf("   - 防止雪崩\n\n");

    printf("4. 消息队列\n");
    printf("   - 消费速率控制\n");
    printf("   - 背压处理\n");
    printf("   - 负载均衡\n\n");

    printf("5. 微服务\n");
    printf("   - 服务间调用限流\n");
    printf("   - 熔断降级\n");
    printf("   - 优雅降级\n");
}

int main(void) {
    printf("========================================\n");
    printf("    漏桶限流器演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_config();
    demo_state();
    demo_wait();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
