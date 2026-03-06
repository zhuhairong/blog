/**
 * 定时器演示程序
 *
 * 功能：
 * - 基本定时器
 * - 重复定时器
 * - 批量操作
 * - 错误处理
 * - 配置管理
 * - 错误信息演示 (timer_strerror)
 * - 配置初始化演示 (timer_config_init, timer_state_init)
 * - 定时器验证演示 (timer_validate)
 * - 统计信息演示 (timer_get_statistics)
 * - 批量操作演示 (timer_batch_set, timer_batch_update, timer_batch_cancel)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/timer.h"

int g_callback_count = 0;

static void timer_callback(void *data) {
    int *count = (int *)data;
    (*count)++;
    printf("定时器回调被调用，计数: %d\n", *count);
}

static void repeating_timer_callback(void *data) {
    g_callback_count++;
    printf("重复定时器回调被调用，计数: %d\n", g_callback_count);
}

static void demo_basic_timer(void) {
    printf("\n=== 演示 1: 基本定时器 ===\n");

    cutils_timer_t timer;
    int callback_count = 0;

    timer_set(&timer, 1000, timer_callback, &callback_count);

    printf("定时器已设置，等待触发...\n");

    while (callback_count == 0) {
        timer_update(&timer);
        usleep(10000);
    }

    printf("定时器触发完成\n");
}

static void demo_extended_timer(void) {
    printf("\n=== 演示 2: 增强版定时器 ===\n");

    cutils_timer_t timer;
    int callback_count = 0;

    timer_error_t error = timer_set_ex(&timer, 1500, timer_callback, &callback_count, NULL, NULL);

    if (error == TIMER_OK) {
        printf("增强版定时器已设置，等待触发...\n");

        while (callback_count == 0) {
            timer_update_ex(&timer, NULL, NULL);
            usleep(10000);
        }

        printf("增强版定时器触发完成\n");
    } else {
        printf("设置定时器失败: 错误码 %d\n", error);
    }
}

static void demo_repeating_timer(void) {
    printf("\n=== 演示 3: 重复定时器 ===\n");

    timer_extended_t timer;

    g_callback_count = 0;

    timer_error_t error = timer_set_repeating(&timer, 1000, 500, 3, repeating_timer_callback, NULL, NULL, NULL);

    if (error == TIMER_OK) {
        printf("重复定时器已设置，等待触发...\n");

        while (g_callback_count < 3) {
            timer_update_extended(&timer, NULL, NULL);
            usleep(10000);
        }

        printf("重复定时器触发完成\n");
    } else {
        printf("设置重复定时器失败: 错误码 %d\n", error);
    }
}

static void demo_timer_state(void) {
    printf("\n=== 演示 4: 定时器状态管理 ===\n");

    cutils_timer_t timer;

    int callback_count = 0;

    timer_set(&timer, 800, timer_callback, &callback_count);

    bool active;
    timer_is_active(&timer, &active);
    printf("定时器初始状态: %s\n", active ? "活跃" : "非活跃");

    timer_error_t error = timer_cancel(&timer, NULL);
    if (error == TIMER_OK) {
        printf("定时器已取消\n");
    }

    timer_is_active(&timer, &active);
    printf("定时器取消后状态: %s\n", active ? "活跃" : "非活跃");
}

static void demo_remaining_time(void) {
    printf("\n=== 演示 5: 剩余时间获取 ===\n");

    cutils_timer_t timer;

    int callback_count = 0;

    timer_set(&timer, 2000, timer_callback, &callback_count);

    uint32_t remaining;
    timer_error_t error = timer_get_remaining(&timer, &remaining, NULL);
    if (error == TIMER_OK) {
        printf("初始剩余时间: %u ms\n", remaining);
    }

    usleep(500000);

    error = timer_get_remaining(&timer, &remaining, NULL);
    if (error == TIMER_OK) {
        printf("500ms后剩余时间: %u ms\n", remaining);
    }

    timer_cancel(&timer, NULL);
}

static void demo_config_file(void) {
    printf("\n=== 演示 6: 配置文件操作 ===\n");

    printf("配置文件操作功能尚未实现\n");
    printf("以下是配置选项说明:\n");
    printf("  - enable_statistics: 启用统计信息\n");
    printf("  - enable_validation: 启用参数验证\n");
    printf("  - max_timers: 最大定时器数量\n");
    printf("  - use_high_resolution: 使用高分辨率计时器\n");
}

static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    cutils_timer_t timer;

    timer_error_t error = timer_set_ex(&timer, 0, NULL, NULL, NULL, NULL);
    if (error != TIMER_OK) {
        printf("测试无效参数: 错误码 %d\n", error);
    }

    error = timer_cancel(&timer, NULL);
    if (error != TIMER_OK) {
        printf("测试取消未激活定时器: 错误码 %d\n", error);
    }
}

static void demo_strerror(void) {
    printf("\n=== 演示 8: 错误信息演示 (timer_strerror) ===\n");

    timer_state_t state;
    timer_state_init(&state);

    printf("初始化状态后，无错误时的信息: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_INVALID_PARAMS;
    printf("设置无效参数错误后: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_TIMER_NULL;
    printf("设置定时器为空错误后: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_DELAY_ZERO;
    printf("设置延迟为零错误后: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_NOT_ACTIVE;
    printf("设置定时器未激活错误后: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_BATCH_OPERATION;
    printf("设置批量操作错误后: %s\n", timer_strerror(&state));

    state.last_error = TIMER_ERROR_MEMORY;
    printf("设置内存错误后: %s\n", timer_strerror(&state));

    printf("\n演示 timer_strerror() 与实际错误结合:\n");
    cutils_timer_t timer;
    timer_state_t op_state;
    timer_state_init(&op_state);

    timer_error_t error = timer_set_ex(&timer, 0, timer_callback, NULL, NULL, &op_state);
    if (error != TIMER_OK) {
        printf("设置定时器失败，错误信息: %s\n", timer_strerror(&op_state));
    }
}

static void demo_config_init(void) {
    printf("\n=== 演示 9: 配置初始化演示 (timer_config_init, timer_state_init) ===\n");

    timer_config_t config;
    timer_config_init(&config);

    printf("配置初始化后的默认值:\n");
    printf("  - enable_statistics: %s\n", config.enable_statistics ? "是" : "否");
    printf("  - enable_validation: %s\n", config.enable_validation ? "是" : "否");
    printf("  - enable_thread_safety: %s\n", config.enable_thread_safety ? "是" : "否");
    printf("  - auto_cleanup: %s\n", config.auto_cleanup ? "是" : "否");
    printf("  - max_timers: %zu\n", config.max_timers);
    printf("  - buffer_size: %zu\n", config.buffer_size);
    printf("  - max_delay: %lu\n", (unsigned long)config.max_delay);
    printf("  - use_high_resolution: %s\n", config.use_high_resolution ? "是" : "否");

    timer_state_t state;
    timer_state_init(&state);

    printf("\n状态初始化后的默认值:\n");
    printf("  - last_error: %d\n", state.last_error);
    printf("  - timer_sets: %zu\n", state.timer_sets);
    printf("  - timer_triggers: %zu\n", state.timer_triggers);
    printf("  - timer_cancels: %zu\n", state.timer_cancels);
    printf("  - is_initialized: %s\n", state.is_initialized ? "是" : "否");
    printf("  - active_timers: %zu\n", state.active_timers);
    printf("  - average_delay: %lu\n", (unsigned long)state.average_delay);

    printf("\n使用初始化配置设置定时器:\n");
    cutils_timer_t timer;
    int callback_count = 0;

    timer_error_t error = timer_set_ex(&timer, 500, timer_callback, &callback_count, &config, &state);
    if (error == TIMER_OK) {
        printf("使用自定义配置设置定时器成功\n");

        while (callback_count == 0) {
            timer_update_ex(&timer, &config, &state);
            usleep(10000);
        }

        printf("定时器触发完成\n");
    }
}

static void demo_validate(void) {
    printf("\n=== 演示 10: 定时器验证演示 (timer_validate) ===\n");

    cutils_timer_t timer;
    timer_state_t state;
    timer_state_init(&state);

    printf("验证未初始化的定时器:\n");
    bool valid = false;
    timer_error_t error = timer_validate(&timer, &valid, &state);
    if (error == TIMER_OK) {
        printf("  验证结果: %s\n", valid ? "有效" : "无效");
    }

    printf("\n验证已设置但未激活的定时器:\n");
    timer_set(&timer, 1000, timer_callback, NULL);
    timer_cancel(&timer, &state);

    error = timer_validate(&timer, &valid, &state);
    if (error == TIMER_OK) {
        printf("  验证结果: %s\n", valid ? "有效" : "无效");
    }

    printf("\n验证活跃的定时器:\n");
    int callback_count = 0;
    timer_set(&timer, 2000, timer_callback, &callback_count);

    error = timer_validate(&timer, &valid, &state);
    if (error == TIMER_OK) {
        printf("  验证结果: %s\n", valid ? "有效" : "无效");
    }

    timer_cancel(&timer, &state);
}

static void demo_statistics(void) {
    printf("\n=== 演示 11: 统计信息演示 (timer_get_statistics) ===\n");

    timer_state_t state;
    timer_state_init(&state);

    printf("初始统计信息:\n");
    timer_get_statistics(&state);
    printf("  - 设置次数: %zu\n", state.timer_sets);
    printf("  - 触发次数: %zu\n", state.timer_triggers);
    printf("  - 取消次数: %zu\n", state.timer_cancels);
    printf("  - 活跃定时器: %zu\n", state.active_timers);

    printf("\n执行一系列定时器操作后:\n");

    cutils_timer_t timer1, timer2, timer3;
    int count1 = 0, count2 = 0;

    timer_set_ex(&timer1, 100, timer_callback, &count1, NULL, &state);
    timer_set_ex(&timer2, 200, timer_callback, &count2, NULL, &state);
    timer_set_ex(&timer3, 5000, timer_callback, NULL, NULL, &state);

    printf("设置3个定时器后:\n");
    timer_get_statistics(&state);
    printf("  - 设置次数: %zu\n", state.timer_sets);
    printf("  - 触发次数: %zu\n", state.timer_triggers);
    printf("  - 取消次数: %zu\n", state.timer_cancels);
    printf("  - 活跃定时器: %zu\n", state.active_timers);

    while (count1 == 0 || count2 == 0) {
        timer_update_ex(&timer1, NULL, &state);
        timer_update_ex(&timer2, NULL, &state);
        usleep(10000);
    }

    timer_cancel(&timer3, &state);

    printf("\n触发2个定时器并取消1个后:\n");
    timer_get_statistics(&state);
    printf("  - 设置次数: %zu\n", state.timer_sets);
    printf("  - 触发次数: %zu\n", state.timer_triggers);
    printf("  - 取消次数: %zu\n", state.timer_cancels);
    printf("  - 活跃定时器: %zu\n", state.active_timers);

    printf("\n重置状态后:\n");
    timer_reset_state(&state);
    printf("  - 设置次数: %zu\n", state.timer_sets);
    printf("  - 触发次数: %zu\n", state.timer_triggers);
    printf("  - 取消次数: %zu\n", state.timer_cancels);
}

static void batch_callback(void *data) {
    int *id = (int *)data;
    printf("批量定时器 %d 被触发\n", *id);
}

static void demo_batch_operations(void) {
    printf("\n=== 演示 12: 批量操作演示 (timer_batch_set/update/cancel) ===\n");

    #define BATCH_SIZE 5
    cutils_timer_t timers[BATCH_SIZE];
    memset(timers, 0, sizeof(timers));  // 初始化定时器数组
    uint32_t delays[BATCH_SIZE] = {200, 400, 600, 800, 1000};
    timer_callback_t callbacks[BATCH_SIZE] = {
        batch_callback, batch_callback, batch_callback, batch_callback, batch_callback
    };
    int ids[BATCH_SIZE] = {1, 2, 3, 4, 5};
    void *datas[BATCH_SIZE] = {&ids[0], &ids[1], &ids[2], &ids[3], &ids[4]};

    timer_config_t config;
    timer_config_init(&config);
    config.enable_statistics = true;

    timer_state_t state;
    timer_state_init(&state);

    printf("使用 timer_batch_set() 批量设置 %d 个定时器:\n", BATCH_SIZE);
    timer_error_t error = timer_batch_set(timers, BATCH_SIZE, delays, callbacks, datas, &config, &state);
    if (error == TIMER_OK) {
        printf("批量设置成功!\n");
        for (int i = 0; i < BATCH_SIZE; i++) {
            printf("  定时器 %d: 延迟 %u ms\n", i + 1, delays[i]);
        }
    } else {
        printf("批量设置失败: 错误码 %d\n", error);
        return;
    }

    printf("\n统计信息:\n");
    printf("  - 活跃定时器: %zu\n", state.active_timers);

    printf("\n使用 timer_batch_update() 等待前3个定时器触发:\n");
    int triggered = 0;
    bool active[BATCH_SIZE] = {true, true, true, true, true};

    while (triggered < 3) {
        timer_batch_update(timers, BATCH_SIZE, &config, &state);

        triggered = 0;
        for (int i = 0; i < BATCH_SIZE; i++) {
            bool is_active;
            timer_is_active(&timers[i], &is_active);
            if (!is_active && active[i]) {
                active[i] = false;
            }
            if (!active[i]) {
                triggered++;
            }
        }
        usleep(10000);
    }

    printf("\n前3个定时器已触发，剩余定时器状态:\n");
    for (int i = 0; i < BATCH_SIZE; i++) {
        bool is_active;
        timer_is_active(&timers[i], &is_active);
        printf("  定时器 %d: %s\n", i + 1, is_active ? "活跃" : "已触发");
    }

    printf("\n使用 timer_batch_cancel() 取消剩余定时器:\n");
    error = timer_batch_cancel(timers, BATCH_SIZE, &state);
    if (error == TIMER_OK) {
        printf("批量取消成功!\n");
    } else {
        printf("批量取消失败: 错误码 %d\n", error);
    }

    printf("\n最终统计信息:\n");
    printf("  - 设置次数: %zu\n", state.timer_sets);
    printf("  - 触发次数: %zu\n", state.timer_triggers);
    printf("  - 取消次数: %zu\n", state.timer_cancels);

    #undef BATCH_SIZE
}

int main(void) {
    printf("========================================\n");
    printf("    定时器演示\n");
    printf("========================================\n");

    demo_basic_timer();
    demo_extended_timer();
    demo_repeating_timer();
    demo_timer_state();
    demo_remaining_time();
    demo_config_file();
    demo_error_handling();
    demo_strerror();
    demo_config_init();
    demo_validate();
    demo_statistics();
    demo_batch_operations();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
