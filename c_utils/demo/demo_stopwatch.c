#include <stdio.h>
#include "../c_utils/stopwatch.h"

// 测试函数，用于测量执行时间
void test_function(void) {
    // 模拟一些计算
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
}

int main() {
    printf("=== Stopwatch Demo ===\n\n");
    
    // 测试基本计时器
    printf("1. 基本计时器测试:\n");
    stopwatch_t sw;
    stopwatch_reset(&sw);
    
    stopwatch_start(&sw);
    // 模拟一些操作
    for (int i = 0; i < 1000000; i++) {
        // 空循环
    }
    stopwatch_stop(&sw);
    
    uint64_t elapsed = stopwatch_elapsed_ms(&sw);
    printf("   执行时间: %llu 毫秒\n", (unsigned long long)elapsed);
    
    // 测试函数执行时间测量
    printf("\n2. 函数执行时间测量:\n");
    stopwatch_reset(&sw);
    stopwatch_start(&sw);
    test_function();
    stopwatch_stop(&sw);
    elapsed = stopwatch_elapsed_ms(&sw);
    printf("   函数执行时间: %llu 毫秒\n", (unsigned long long)elapsed);
    
    // 测试多次测量
    printf("\n3. 多次测量测试:\n");
    uint64_t times[5];
    for (int i = 0; i < 5; i++) {
        stopwatch_reset(&sw);
        stopwatch_start(&sw);
        // 模拟不同的操作时间
        for (int j = 0; j < 500000 * (i + 1); j++) {
            // 空循环
        }
        stopwatch_stop(&sw);
        times[i] = stopwatch_elapsed_ms(&sw);
        printf("   测量 %d: %llu 毫秒\n", i + 1, (unsigned long long)times[i]);
    }
    
    // 测试连续测量
    printf("\n4. 连续测量测试:\n");
    stopwatch_reset(&sw);
    
    // 第一次测量
    stopwatch_start(&sw);
    for (int i = 0; i < 500000; i++) {
        // 空循环
    }
    stopwatch_stop(&sw);
    elapsed = stopwatch_elapsed_ms(&sw);
    printf("   第一次测量: %llu 毫秒\n", (unsigned long long)elapsed);
    
    // 第二次测量（不重置，累计时间）
    stopwatch_start(&sw);
    for (int i = 0; i < 500000; i++) {
        // 空循环
    }
    stopwatch_stop(&sw);
    elapsed = stopwatch_elapsed_ms(&sw);
    printf("   累计测量: %llu 毫秒\n", (unsigned long long)elapsed);
    
    // 重置后再次测量
    stopwatch_reset(&sw);
    stopwatch_start(&sw);
    for (int i = 0; i < 500000; i++) {
        // 空循环
    }
    stopwatch_stop(&sw);
    elapsed = stopwatch_elapsed_ms(&sw);
    printf("   重置后测量: %llu 毫秒\n", (unsigned long long)elapsed);
    
    printf("\n=== Stopwatch Demo 完成 ===\n");
    return 0;
}