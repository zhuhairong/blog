/**
 * 漏桶限流器演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/rate_limiter_leaky.h"

static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    leaky_bucket_t bucket;
    leaky_init(&bucket, 10.0, 2.0);

    printf("创建漏桶: 容量=10, 速率=2/秒\n");

    printf("\n尝试消耗 15 个请求:\n");
    for (int i = 1; i <= 15; i++) {
        bool allowed = leaky_consume(&bucket, 1.0);
        printf("  请求 %2d: %s\n", i, allowed ? "允许" : "拒绝");
    }

    leaky_bucket_state_t state;
    leaky_get_state(&bucket, &state, NULL);
    printf("\n当前水位: %.1f\n", state.current_water);
}

static void demo_rate_control(void) {
    printf("\n=== 演示 2: 速率控制 ===\n");

    leaky_bucket_t bucket;
    leaky_bucket_config_t config = leaky_bucket_default_config();
    config.capacity = 5.0;
    config.rate = 1.0;

    leaky_bucket_error_t error;
    leaky_init_ex(&bucket, &config, &error);

    printf("漏桶配置: 容量=5, 速率=1/秒\n");

    printf("\n模拟 5 秒的请求:\n");
    for (int sec = 0; sec < 5; sec++) {
        int allowed = 0;
        int rejected = 0;

        for (int i = 0; i < 3; i++) {
            if (leaky_consume(&bucket, 1.0)) {
                allowed++;
            } else {
                rejected++;
            }
        }

        leaky_bucket_state_t state;
        leaky_get_state(&bucket, &state, NULL);
        printf("  第 %2d 秒: 允许=%d, 拒绝=%d, 水位=%.1f\n",
               sec + 1, allowed, rejected, state.current_water);
        sleep(1);
    }
}

static void demo_burst(void) {
    printf("\n=== 演示 3: 突发流量处理 ===\n");

    leaky_bucket_t bucket;
    leaky_init(&bucket, 20.0, 5.0);

    printf("漏桶配置: 容量=20, 速率=5/秒\n");

    printf("\n突发 25 个请求:\n");
    int allowed = 0, rejected = 0;
    for (int i = 0; i < 25; i++) {
        if (leaky_consume(&bucket, 1.0)) {
            allowed++;
        } else {
            rejected++;
        }
    }
    printf("  允许: %d, 拒绝: %d\n", allowed, rejected);

    leaky_bucket_state_t state;
    leaky_get_state(&bucket, &state, NULL);
    printf("  当前水位: %.1f\n", state.current_water);
}

static void demo_different_weights(void) {
    printf("\n=== 演示 4: 不同权重请求 ===\n");

    leaky_bucket_t bucket;
    leaky_init(&bucket, 10.0, 2.0);

    printf("漏桶配置: 容量=10, 速率=2/秒\n");

    printf("\n不同权重的请求:\n");

    bool result = leaky_consume(&bucket, 3.0);
    printf("  消耗权重 3: %s\n", result ? "允许" : "拒绝");
    leaky_bucket_state_t state;
    leaky_get_state(&bucket, &state, NULL);
    printf("  水位: %.1f\n", state.current_water);

    result = leaky_consume(&bucket, 5.0);
    printf("  消耗权重 5: %s\n", result ? "允许" : "拒绝");
    leaky_get_state(&bucket, &state, NULL);
    printf("  水位: %.1f\n", state.current_water);

    result = leaky_consume(&bucket, 4.0);
    printf("  消耗权重 4: %s\n", result ? "允许" : "拒绝");
    leaky_get_state(&bucket, &state, NULL);
    printf("  水位: %.1f\n", state.current_water);

    result = leaky_consume(&bucket, 1.0);
    printf("  消耗权重 1: %s\n", result ? "允许" : "拒绝");
    leaky_get_state(&bucket, &state, NULL);
    printf("  水位: %.1f\n", state.current_water);
}

static void demo_state_info(void) {
    printf("\n=== 演示 5: 状态信息 ===\n");

    leaky_bucket_t bucket;
    leaky_init(&bucket, 100.0, 10.0);

    leaky_consume(&bucket, 50.0);

    leaky_bucket_state_t state;
    leaky_get_state(&bucket, &state, NULL);

    printf("漏桶状态:\n");
    printf("  当前水量: %.1f\n", state.current_water);
    printf("  可用容量: %.1f\n", state.available_capacity);
    printf("  填充水平: %.1f%%\n", state.fill_level * 100);
    printf("  是否已满: %s\n", state.is_full ? "是" : "否");
    printf("  是否为空: %s\n", state.is_empty ? "是" : "否");

    printf("\n重置漏桶...\n");
    leaky_reset(&bucket, NULL);

    leaky_get_state(&bucket, &state, NULL);
    printf("重置后水量: %.1f\n", state.current_water);
}

int main(void) {
    printf("========================================\n");
    printf("    漏桶限流器演示\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_rate_control();
    demo_burst();
    demo_different_weights();
    demo_state_info();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
