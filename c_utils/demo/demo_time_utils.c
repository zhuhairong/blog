#include <stdio.h>
#include "../c_utils/time_utils.h"

int main() {
    printf("=== Time Utils Demo ===\n\n");
    
    // 测试基本时间获取
    printf("1. 基本时间获取:\n");
    uint64_t now_ms = time_now_ms();
    uint64_t now_us = time_now_us();
    printf("   当前时间（毫秒）: %llu\n", (unsigned long long)now_ms);
    printf("   当前时间（微秒）: %llu\n", (unsigned long long)now_us);
    
    // 测试时间格式化
    printf("\n2. 时间格式化:\n");
    char time_buf[128];
    time_format_now(time_buf, sizeof(time_buf));
    printf("   当前时间: %s\n", time_buf);
    
    // 测试睡眠
    printf("\n3. 睡眠测试:\n");
    printf("   开始睡眠 100 毫秒...\n");
    time_sleep_ms(100); // 睡眠100毫秒
    printf("   睡眠结束\n");
    
    // 测试时间差（简单计算）
    printf("\n4. 简单时间差计算:\n");
    uint64_t start = time_now_ms();
    time_sleep_ms(100); // 睡眠100毫秒
    uint64_t end = time_now_ms();
    uint64_t diff_ms = end - start;
    printf("   开始时间: %llu\n", (unsigned long long)start);
    printf("   结束时间: %llu\n", (unsigned long long)end);
    printf("   时间差（毫秒）: %llu\n", (unsigned long long)diff_ms);
    
    printf("\n=== Time Utils Demo 完成 ===\n");
    return 0;
}