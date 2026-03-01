#include <stdio.h>
#include "../c_utils/stats.h"

int main() {
    printf("=== Stats Demo ===\n\n");
    
    // 测试数据
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    
    printf("测试数据: ");
    for (size_t i = 0; i < n; i++) {
        printf("%g ", data[i]);
    }
    printf("\n\n");
    
    // 测试基本统计计算
    printf("1. 基本统计计算:\n");
    stats_t basic_stats = stats_compute(data, n);
    printf("   最小值: %g\n", basic_stats.min);
    printf("   最大值: %g\n", basic_stats.max);
    printf("   平均值: %g\n", basic_stats.mean);
    printf("   方差: %g\n", basic_stats.variance);
    printf("   标准差: %g\n", basic_stats.stddev);
    
    // 测试不同的数据集
    printf("\n2. 测试不同的数据集:\n");
    
    // 测试负数和零
    double data2[] = {-5.0, -3.0, 0.0, 2.0, 4.0};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    
    printf("   测试数据 (负数和零): ");
    for (size_t i = 0; i < n2; i++) {
        printf("%g ", data2[i]);
    }
    printf("\n");
    
    stats_t stats2 = stats_compute(data2, n2);
    printf("   最小值: %g\n", stats2.min);
    printf("   最大值: %g\n", stats2.max);
    printf("   平均值: %g\n", stats2.mean);
    printf("   方差: %g\n", stats2.variance);
    printf("   标准差: %g\n", stats2.stddev);
    
    // 测试相同值的数据集
    double data3[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    size_t n3 = sizeof(data3) / sizeof(data3[0]);
    
    printf("\n   测试数据 (相同值): ");
    for (size_t i = 0; i < n3; i++) {
        printf("%g ", data3[i]);
    }
    printf("\n");
    
    stats_t stats3 = stats_compute(data3, n3);
    printf("   最小值: %g\n", stats3.min);
    printf("   最大值: %g\n", stats3.max);
    printf("   平均值: %g\n", stats3.mean);
    printf("   方差: %g\n", stats3.variance);
    printf("   标准差: %g\n", stats3.stddev);
    
    // 测试单元素数据集
    double data4[] = {42.0};
    size_t n4 = sizeof(data4) / sizeof(data4[0]);
    
    printf("\n   测试数据 (单元素): %g\n", data4[0]);
    
    stats_t stats4 = stats_compute(data4, n4);
    printf("   最小值: %g\n", stats4.min);
    printf("   最大值: %g\n", stats4.max);
    printf("   平均值: %g\n", stats4.mean);
    printf("   方差: %g\n", stats4.variance);
    printf("   标准差: %g\n", stats4.stddev);
    
    printf("\n=== Stats Demo 完成 ===\n");
    return 0;
}