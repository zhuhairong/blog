#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/cpu_usage.h"

void test_cpu_usage_init() {
    TEST(CpuUsage_Init);
    cpu_usage_error_t error;
    bool result = cpu_usage_init(&error);
    EXPECT_TRUE(result);
}

void test_cpu_usage_get_core_count() {
    TEST(CpuUsage_GetCoreCount);
    int count = cpu_usage_get_core_count();
    EXPECT_TRUE(count > 0);
}

void test_cpu_usage_get_process_time() {
    TEST(CpuUsage_GetProcessTime);
    cpu_usage_error_t error;
    double user_time = 0.0;
    double system_time = 0.0;
    
    double total = cpu_usage_get_process_time(&user_time, &system_time, &error);
    EXPECT_TRUE(total >= 0.0);
}

void test_cpu_usage_get_overall() {
    TEST(CpuUsage_GetOverall);
    cpu_usage_error_t error;
    cpu_usage_stats_t stats;
    
    bool result = cpu_usage_get_overall(&stats, &error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(stats.total >= 0.0);
    EXPECT_TRUE(stats.total <= 100.0);
}

void test_cpu_usage_get_core() {
    TEST(CpuUsage_GetCore);
    cpu_usage_error_t error;
    cpu_usage_stats_t stats;
    
    int core_count = cpu_usage_get_core_count();
    if (core_count > 0) {
        bool result = cpu_usage_get_core(0, &stats, &error);
        EXPECT_TRUE(result);
    }
}

void test_cpu_usage_is_core_valid() {
    TEST(CpuUsage_IsCoreValid);
    int core_count = cpu_usage_get_core_count();
    
    EXPECT_TRUE(cpu_usage_is_core_valid(0));
    EXPECT_FALSE(cpu_usage_is_core_valid(-1));
    EXPECT_FALSE(cpu_usage_is_core_valid(core_count + 100));
}

void test_cpu_usage_refresh() {
    TEST(CpuUsage_Refresh);
    cpu_usage_error_t error;
    bool result = cpu_usage_refresh(&error);
    EXPECT_TRUE(result);
}

void test_cpu_usage_strerror() {
    TEST(CpuUsage_Strerror);
    const char* msg = cpu_usage_strerror(CPU_USAGE_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = cpu_usage_strerror(CPU_USAGE_ERROR_INVALID_CORE);
    EXPECT_TRUE(msg != NULL);
}

void test_cpu_usage_get_time_stats() {
    TEST(CpuUsage_GetTimeStats);
    cpu_usage_error_t error;
    cpu_time_stats_t stats;
    
    bool result = cpu_usage_get_time_stats(0, &stats, &error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(stats.idle >= 0.0);
}

void test_cpu_usage_multiple_refresh() {
    TEST(CpuUsage_MultipleRefresh);
    cpu_usage_error_t error;
    
    for (int i = 0; i < 3; i++) {
        bool result = cpu_usage_refresh(&error);
        EXPECT_TRUE(result);
    }
}

int main() {
    test_cpu_usage_init();
    test_cpu_usage_get_core_count();
    test_cpu_usage_get_process_time();
    test_cpu_usage_get_overall();
    test_cpu_usage_get_core();
    test_cpu_usage_is_core_valid();
    test_cpu_usage_refresh();
    test_cpu_usage_strerror();
    test_cpu_usage_get_time_stats();
    test_cpu_usage_multiple_refresh();

    return 0;
}
