/**
 * 定时器演示程序
 *
 * 功能：
 * - 基本定时器
 * - 重复定时器
 * - 批量操作
 * - 错误处理
 * - 配置管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/timer.h"

// 全局变量，用于演示定时器回调
int g_callback_count = 0;

// 基本定时器回调函数
static void timer_callback(void *data) {
    int *count = (int *)data;
    (*count)++;
    printf("定时器回调被调用，计数: %d\n", *count);
}

// 重复定时器回调函数
static void repeating_timer_callback(void *data) {
    g_callback_count++;
    printf("重复定时器回调被调用，计数: %d\n", g_callback_count);
}

// 演示 1: 基本定时器
static void demo_basic_timer(void) {
    printf("\n=== 演示 1: 基本定时器 ===\n");

    cutils_timer_t timer;
    int callback_count = 0;

    // 设置定时器，1秒后触发
    timer_set(&timer, 1000, timer_callback, &callback_count);

    printf("定时器已设置，等待触发...\n");

    // 模拟主循环
    while (callback_count == 0) {
        timer_update(&timer);
        usleep(10000); // 10ms
    }

    printf("定时器触发完成\n");
}

// 演示 2: 增强版定时器
static void demo_extended_timer(void) {
    printf("\n=== 演示 2: 增强版定时器 ===\n");

    cutils_timer_t timer;
    int callback_count = 0;

    // 设置定时器，1.5秒后触发
    timer_error_t error = timer_set_ex(&timer, 1500, timer_callback, &callback_count, NULL, NULL);

    if (error == TIMER_OK) {
        printf("增强版定时器已设置，等待触发...\n");

        // 模拟主循环
        while (callback_count == 0) {
            timer_update_ex(&timer, NULL, NULL);
            usleep(10000); // 10ms
        }

        printf("增强版定时器触发完成\n");
    } else {
        printf("设置定时器失败: 错误码 %d\n", error);
    }
}

// 演示 3: 重复定时器
static void demo_repeating_timer(void) {
    printf("\n=== 演示 3: 重复定时器 ===\n");

    timer_extended_t timer;

    g_callback_count = 0;

    // 设置重复定时器，初始延迟1秒，间隔500ms，最多重复3次
    timer_error_t error = timer_set_repeating(&timer, 1000, 500, 3, repeating_timer_callback, NULL, NULL, NULL);

    if (error == TIMER_OK) {
        printf("重复定时器已设置，等待触发...\n");

        // 模拟主循环
        while (g_callback_count < 3) {
            timer_update_extended(&timer, NULL, NULL);
            usleep(10000); // 10ms
        }

        printf("重复定时器触发完成\n");
    } else {
        printf("设置重复定时器失败: 错误码 %d\n", error);
    }
}

// 演示 4: 批量操作
static void batch_callback(void *data) {
    int *id = (int *)data;
    printf("批量定时器 %d 被触发\n", *id);
}

static void demo_batch_operations(void) {
    printf("\n=== 演示 4: 批量操作 ===\n");

    #define BATCH_SIZE 3
    cutils_timer_t timers[BATCH_SIZE];
    int ids[BATCH_SIZE] = {1, 2, 3};

    // 逐个设置定时器
    for (int i = 0; i < BATCH_SIZE; i++) {
        int *id = &ids[i];
        timer_set(&timers[i], 500 * (i + 1), batch_callback, id);
        printf("设置定时器 %d，延迟 %d ms\n", i + 1, 500 * (i + 1));
    }

    printf("批量定时器已设置，等待触发...\n");

    // 模拟主循环
    int triggered_count = 0;
    while (triggered_count < BATCH_SIZE) {
        for (int i = 0; i < BATCH_SIZE; i++) {
            timer_update(&timers[i]);
        }
        
        // 检查是否有定时器已触发
        triggered_count = 0;
        for (int i = 0; i < BATCH_SIZE; i++) {
            bool active;
            timer_is_active(&timers[i], &active);
            if (!active) {
                triggered_count++;
            }
        }
        
        usleep(10000); // 10ms
    }

    printf("批量定时器触发完成\n");
}

// 演示 5: 定时器状态管理
static void demo_timer_state(void) {
    printf("\n=== 演示 5: 定时器状态管理 ===\n");

    cutils_timer_t timer;

    int callback_count = 0;

    // 设置定时器
    timer_set(&timer, 800, timer_callback, &callback_count);

    // 检查定时器状态
    bool active;
    timer_is_active(&timer, &active);
    printf("定时器初始状态: %s\n", active ? "活跃" : "非活跃");

    // 取消定时器
    timer_error_t error = timer_cancel(&timer, NULL);
    if (error == TIMER_OK) {
        printf("定时器已取消\n");
    }

    // 再次检查状态
    timer_is_active(&timer, &active);
    printf("定时器取消后状态: %s\n", active ? "活跃" : "非活跃");
}

// 演示 6: 剩余时间获取
static void demo_remaining_time(void) {
    printf("\n=== 演示 6: 剩余时间获取 ===\n");

    cutils_timer_t timer;

    int callback_count = 0;

    // 设置定时器，2秒后触发
    timer_set(&timer, 2000, timer_callback, &callback_count);

    // 获取剩余时间
    uint32_t remaining;
    timer_error_t error = timer_get_remaining(&timer, &remaining, NULL);
    if (error == TIMER_OK) {
        printf("初始剩余时间: %u ms\n", remaining);
    }

    // 等待一段时间
    usleep(500000); // 500ms

    // 再次获取剩余时间
    error = timer_get_remaining(&timer, &remaining, NULL);
    if (error == TIMER_OK) {
        printf("500ms后剩余时间: %u ms\n", remaining);
    }

    // 取消定时器
    timer_cancel(&timer, NULL);
}

// 演示 7: 配置文件操作
static void demo_config_file(void) {
    printf("\n=== 演示 7: 配置文件操作 ===\n");

    // 注意：配置文件操作功能尚未实现
    printf("配置文件操作功能尚未实现\n");
    printf("以下是配置选项说明:\n");
    printf("  - enable_statistics: 启用统计信息\n");
    printf("  - enable_validation: 启用参数验证\n");
    printf("  - max_timers: 最大定时器数量\n");
    printf("  - use_high_resolution: 使用高分辨率计时器\n");
}

// 演示 8: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 8: 错误处理 ===\n");

    cutils_timer_t timer;

    // 测试无效参数
    timer_error_t error = timer_set_ex(&timer, 0, NULL, NULL, NULL, NULL);
    if (error != TIMER_OK) {
        printf("测试无效参数: 错误码 %d\n", error);
    }

    // 测试取消未激活的定时器
    error = timer_cancel(&timer, NULL);
    if (error != TIMER_OK) {
        printf("测试取消未激活定时器: 错误码 %d\n", error);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    定时器演示\n");
    printf("========================================\n");

    demo_basic_timer();
    demo_extended_timer();
    demo_repeating_timer();
    demo_batch_operations();
    demo_timer_state();
    demo_remaining_time();
    demo_config_file();
    demo_error_handling();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
