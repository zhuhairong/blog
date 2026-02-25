/**
 * @file demo_cpu_usage.c
 * @brief CPU 使用率统计演示
 *
 * 本演示展示了 CPU 使用率的监控和统计，包括：
 * - 系统 CPU 使用率
 * - 进程 CPU 使用率
 * - 多核 CPU 统计
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/cpu_usage.h"

// 演示 1: 初始化
static void demo_init(void) {
    printf("\n=== 演示 1: 初始化 ===\n");

    cpu_usage_error_t error;
    
    printf("初始化 CPU 使用率统计...\n");
    if (cpu_usage_init(&error)) {
        printf("初始化成功\n");
    } else {
        printf("初始化失败，错误码: %d\n", error);
    }
}

// 演示 2: 进程 CPU 时间
static void demo_process_time(void) {
    printf("\n=== 演示 2: 进程 CPU 时间 ===\n");

    double user_time, system_time;
    cpu_usage_error_t error;

    printf("获取当前进程 CPU 时间...\n");
    double total = cpu_usage_get_process_time(&user_time, &system_time, &error);
    if (total > 0) {
        printf("  用户时间: %.3f 秒\n", user_time);
        printf("  系统时间: %.3f 秒\n", system_time);
        printf("  总时间: %.3f 秒\n", total);
    } else {
        printf("获取失败，错误码: %d\n", error);
    }

    // 执行一些计算工作
    printf("\n执行一些计算工作...\n");
    volatile long sum = 0;
    for (int i = 0; i < 100000000; i++) {
        sum += i;
    }

    // 再次获取
    printf("再次获取 CPU 时间...\n");
    total = cpu_usage_get_process_time(&user_time, &system_time, &error);
    if (total > 0) {
        printf("  用户时间: %.3f 秒\n", user_time);
        printf("  系统时间: %.3f 秒\n", system_time);
        printf("  总时间: %.3f 秒\n", total);
    }
}

// 演示 3: 系统 CPU 统计
static void demo_system_stats(void) {
    printf("\n=== 演示 3: 系统 CPU 统计 ===\n");

    cpu_time_stats_t time_stats;
    cpu_usage_error_t error;

    printf("获取系统 CPU 时间统计...\n");
    if (cpu_usage_get_time_stats(0, &time_stats, &error)) {
        printf("  用户时间: %.2f%%\n", time_stats.user);
        printf("  系统时间: %.2f%%\n", time_stats.system);
        printf("  空闲时间: %.2f%%\n", time_stats.idle);
        printf("  IO 等待: %.2f%%\n", time_stats.io_wait);
        printf("  硬中断: %.2f%%\n", time_stats.irq);
        printf("  软中断: %.2f%%\n", time_stats.softirq);
    } else {
        printf("获取失败，错误码: %d\n", error);
    }

    printf("\n获取总体 CPU 使用率...\n");
    cpu_usage_stats_t stats;
    if (cpu_usage_get_overall(&stats, &error)) {
        printf("  总使用率: %.2f%%\n", stats.total);
        printf("  用户态: %.2f%%\n", stats.user);
        printf("  系统态: %.2f%%\n", stats.system);
        printf("  空闲: %.2f%%\n", stats.idle);
    } else {
        printf("获取失败，错误码: %d\n", error);
    }
}

// 演示 4: 多核 CPU 统计
static void demo_multi_core(void) {
    printf("\n=== 演示 4: 多核 CPU 统计 ===\n");

    int core_count = cpu_usage_get_core_count();
    printf("系统 CPU 核心数: %d\n", core_count);

    if (core_count > 1) {
        printf("\n各核心使用率:\n");
        for (int i = 0; i < core_count && i < 4; i++) {
            cpu_usage_stats_t stats;
            cpu_usage_error_t error;
            
            if (cpu_usage_get_core(i + 1, &stats, &error)) {
                printf("  CPU %d: 总计 %.1f%% (用户 %.1f%%, 系统 %.1f%%, 空闲 %.1f%%)\n",
                       i, stats.total, stats.user, stats.system, stats.idle);
            }
        }
        if (core_count > 4) {
            printf("  ... 还有 %d 个核心\n", core_count - 4);
        }
    }
}

// 演示 5: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CPU_USAGE_OK (%d): 成功\n", CPU_USAGE_OK);
    printf("  CPU_USAGE_ERROR_INVALID_CORE (%d): 无效核心\n", CPU_USAGE_ERROR_INVALID_CORE);
    printf("  CPU_USAGE_ERROR_OPERATION_FAILED (%d): 操作失败\n", CPU_USAGE_ERROR_OPERATION_FAILED);
    printf("  CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED (%d): 平台不支持\n", CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED);
    printf("  CPU_USAGE_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CPU_USAGE_ERROR_MEMORY_ALLOC);
    printf("  CPU_USAGE_ERROR_INSUFFICIENT_DATA (%d): 数据不足\n", CPU_USAGE_ERROR_INSUFFICIENT_DATA);
}

// 演示 6: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("CPU 使用率监控的应用场景:\n\n");

    printf("1. 系统监控:\n");
    printf("   - 实时显示 CPU 使用率\n");
    printf("   - 检测 CPU 过载\n");
    printf("   - 生成性能报告\n\n");

    printf("2. 进程管理:\n");
    printf("   - 监控进程资源消耗\n");
    printf("   - 检测异常进程\n");
    printf("   - 资源限制和调度\n\n");

    printf("3. 性能分析:\n");
    printf("   - 识别 CPU 密集型代码\n");
    printf("   - 优化热点函数\n");
    printf("   - 评估算法效率\n\n");

    printf("4. 负载均衡:\n");
    printf("   - 监控各核心负载\n");
    printf("   - 动态调整任务分配\n");
    printf("   - 避免单核过载\n");
}

// 演示 7: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 7: 最佳实践 ===\n");

    printf("CPU 使用率监控最佳实践:\n\n");

    printf("1. 采样频率:\n");
    printf("   - 系统监控: 1-5 秒\n");
    printf("   - 进程监控: 100ms-1 秒\n");
    printf("   - 性能分析: 10-100ms\n\n");

    printf("2. 数据平滑:\n");
    printf("   - 使用移动平均减少抖动\n");
    printf("   - 设置合理的阈值\n");
    printf("   - 避免过度反应\n\n");

    printf("3. 多核处理:\n");
    printf("   - 区分总体和单核使用率\n");
    printf("   - 关注热点核心\n");
    printf("   - 考虑线程亲和性\n\n");

    printf("4. 资源消耗:\n");
    printf("   - 监控本身也会消耗 CPU\n");
    printf("   - 避免过于频繁的采样\n");
    printf("   - 使用高效的统计方法\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CPU 使用率统计演示程序\n");
    printf("========================================\n");

    demo_init();
    demo_process_time();
    demo_system_stats();
    demo_multi_core();
    demo_error_handling();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
