/**
 * @file demo_cpu_affinity.c
 * @brief CPU 亲和性演示
 *
 * 本演示展示了 CPU 亲和性的设置和查询，包括：
 * - 获取系统 CPU 信息
 * - 设置/获取线程的 CPU 亲和性
 * - 绑定线程到特定 CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/cpu_affinity.h"

// 演示 1: 系统 CPU 信息
static void demo_cpu_info(void) {
    printf("\n=== 演示 1: 系统 CPU 信息 ===\n");

    int cpu_count = cpu_affinity_get_cpu_count();
    printf("系统 CPU 数量: %d\n", cpu_count);

    if (cpu_count > 0) {
        printf("\nCPU 有效性检查:\n");
        for (int i = 0; i < cpu_count && i < 8; i++) {
            bool valid = cpu_affinity_is_cpu_valid(i);
            printf("  CPU %d: %s\n", i, valid ? "有效" : "无效");
        }
        if (cpu_count > 8) {
            printf("  ... 还有 %d 个 CPU\n", cpu_count - 8);
        }
    }
}

// 演示 2: 当前线程 CPU 亲和性
static void demo_current_thread(void) {
    printf("\n=== 演示 2: 当前线程 CPU 亲和性 ===\n");

    cpu_affinity_error_t error;
    int cpu_id;

    // 获取当前亲和性
    if (cpu_affinity_get(&cpu_id, &error)) {
        printf("当前线程绑定的 CPU: %d\n", cpu_id);
    } else {
        printf("获取 CPU 亲和性失败，错误码: %d\n", error);
    }

    // 尝试绑定到 CPU 0
    printf("\n尝试绑定到 CPU 0...\n");
    if (cpu_affinity_set(0, &error)) {
        printf("绑定成功\n");

        // 验证绑定
        if (cpu_affinity_get(&cpu_id, &error)) {
            printf("验证: 当前线程绑定的 CPU: %d\n", cpu_id);
        }
    } else {
        printf("绑定失败，错误码: %d\n", error);
        printf("(可能需要 root 权限或不支持此平台)\n");
    }
}

// 演示 3: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 3: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CPU_AFFINITY_OK (%d): 成功\n", CPU_AFFINITY_OK);
    printf("  CPU_AFFINITY_ERROR_INVALID_CPU (%d): 无效 CPU\n", CPU_AFFINITY_ERROR_INVALID_CPU);
    printf("  CPU_AFFINITY_ERROR_INVALID_THREAD (%d): 无效线程\n", CPU_AFFINITY_ERROR_INVALID_THREAD);
    printf("  CPU_AFFINITY_ERROR_OPERATION_FAILED (%d): 操作失败\n", CPU_AFFINITY_ERROR_OPERATION_FAILED);
    printf("  CPU_AFFINITY_ERROR_PLATFORM_UNSUPPORTED (%d): 平台不支持\n", CPU_AFFINITY_ERROR_PLATFORM_UNSUPPORTED);
    printf("  CPU_AFFINITY_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CPU_AFFINITY_ERROR_MEMORY_ALLOC);

    // 测试无效 CPU
    printf("\n测试无效 CPU:\n");
    cpu_affinity_error_t error;
    bool result = cpu_affinity_set(-1, &error);
    printf("  绑定到 CPU -1: %s, 错误码: %d\n", result ? "成功" : "失败", error);

    int cpu_count = cpu_affinity_get_cpu_count();
    result = cpu_affinity_set(cpu_count + 100, &error);
    printf("  绑定到无效 CPU %d: %s, 错误码: %d\n", cpu_count + 100, result ? "成功" : "失败", error);
}

// 演示 4: CPU 掩码操作
static void demo_cpu_mask(void) {
    printf("\n=== 演示 4: CPU 掩码操作 ===\n");

    printf("CPU 掩码类型: uint64_t\n");
    printf("  大小: %zu 位\n", sizeof(cpu_mask_t) * 8);
    printf("  可表示的 CPU 数量: %zu\n", sizeof(cpu_mask_t) * 8);

    // 演示掩码位操作
    printf("\n掩码位操作示例:\n");
    cpu_mask_t mask = 0;

    // 设置位 0, 2, 4
    mask |= (1ULL << 0);
    mask |= (1ULL << 2);
    mask |= (1ULL << 4);

    printf("  初始掩码: 0x%016lX\n", (unsigned long)mask);
    printf("  启用的 CPU: ");
    for (int i = 0; i < 8; i++) {
        if (mask & (1ULL << i)) {
            printf("CPU%d ", i);
        }
    }
    printf("\n");

    // 清除位 2
    mask &= ~(1ULL << 2);
    printf("  清除 CPU 2 后: 0x%016lX\n", (unsigned long)mask);
}

// 演示 5: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("CPU 亲和性的应用场景:\n\n");

    printf("1. 性能优化:\n");
    printf("   - 将计算密集型线程绑定到特定 CPU\n");
    printf("   - 减少缓存失效和上下文切换\n");
    printf("   - 提高缓存命中率\n\n");

    printf("2. 实时系统:\n");
    printf("   - 确保关键任务在指定 CPU 上运行\n");
    printf("   - 避免被其他任务抢占\n");
    printf("   - 保证响应时间\n\n");

    printf("3. 负载均衡:\n");
    printf("   - 将不同线程分配到不同 CPU\n");
    printf("   - 避免 CPU 过载\n");
    printf("   - 提高系统吞吐量\n\n");

    printf("4. NUMA 系统:\n");
    printf("   - 将线程绑定到本地内存节点\n");
    printf("   - 减少跨节点内存访问\n");
    printf("   - 提高内存访问速度\n");
}

// 演示 6: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 6: 最佳实践 ===\n");

    printf("CPU 亲和性最佳实践:\n\n");

    printf("1. 线程绑定:\n");
    printf("   - 每个线程绑定到不同 CPU\n");
    printf("   - 避免多个线程竞争同一 CPU\n");
    printf("   - 考虑 CPU 缓存层次结构\n\n");

    printf("2. 动态调整:\n");
    printf("   - 根据负载动态调整绑定\n");
    printf("   - 监控 CPU 使用率\n");
    printf("   - 避免热点 CPU\n\n");

    printf("3. 错误处理:\n");
    printf("   - 检查平台支持\n");
    printf("   - 处理权限问题\n");
    printf("   - 提供降级方案\n\n");

    printf("4. 调试技巧:\n");
    printf("   - 使用 taskset 命令验证\n");
    printf("   - 监控 /proc/stat\n");
    printf("   - 记录绑定信息到日志\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CPU 亲和性演示程序\n");
    printf("========================================\n");

    demo_cpu_info();
    demo_current_thread();
    demo_error_handling();
    demo_cpu_mask();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
