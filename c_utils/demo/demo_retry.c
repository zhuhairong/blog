/**
 * 重试机制演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "../c_utils/retry.h"

static int success_attempt = 3;

static bool demo_callback(void *user_data, int attempt, retry_error_t *error) {
    (void)user_data;
    (void)error;
    printf("  尝试 %d/%d\n", attempt, success_attempt + 2);
    return attempt >= success_attempt;
}

static void demo_basic_retry(void) {
    printf("\n=== 演示 1: 基本重试 ===\n");

    retry_policy_t policy;
    retry_init(&policy, 5, 100);

    printf("配置: 最多 %d 次重试, 基础延迟 %dms\n", 
           policy.config.max_attempts, policy.config.base_delay_ms);

    success_attempt = 3;
    retry_error_t error;
    bool success = retry_execute(&policy, demo_callback, NULL, &error);

    printf("执行结果: %s\n", success ? "成功" : "失败");
    printf("总尝试次数: %d\n", retry_get_attempts(&policy));
}

static void demo_immediate_success(void) {
    printf("\n=== 演示 2: 首次成功 ===\n");

    retry_policy_t policy;
    retry_init(&policy, 5, 100);

    success_attempt = 0;
    retry_error_t error;
    bool success = retry_execute(&policy, demo_callback, NULL, &error);

    printf("执行结果: %s\n", success ? "成功" : "失败");
    printf("总尝试次数: %d\n", retry_get_attempts(&policy));
}

static void demo_exhausted(void) {
    printf("\n=== 演示 3: 重试耗尽 ===\n");

    retry_policy_t policy;
    retry_init(&policy, 3, 50);

    success_attempt = 100;
    retry_error_t error;
    bool success = retry_execute(&policy, demo_callback, NULL, &error);

    printf("执行结果: %s\n", success ? "成功" : "失败");
    printf("总尝试次数: %d\n", retry_get_attempts(&policy));
    printf("是否耗尽: %s\n", retry_is_exhausted(&policy) ? "是" : "否");
}

int main(void) {
    srand(time(NULL));

    printf("========================================\n");
    printf("    重试机制演示\n");
    printf("========================================\n");

    demo_basic_retry();
    demo_immediate_success();
    demo_exhausted();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
