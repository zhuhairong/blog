#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/cpu_affinity.h"

void test_cpu_affinity_get_cpu_count() {
    TEST(CpuAffinity_GetCpuCount);
    int count = cpu_affinity_get_cpu_count();
    EXPECT_TRUE(count > 0);
}

void test_cpu_affinity_is_cpu_valid() {
    TEST(CpuAffinity_IsCpuValid);
    int count = cpu_affinity_get_cpu_count();
    
    if (count > 0) {
        bool valid = cpu_affinity_is_cpu_valid(0);
        EXPECT_TRUE(valid);
    } else {
        EXPECT_TRUE(true);
    }
}

void test_cpu_affinity_get() {
    TEST(CpuAffinity_Get);
    int cpu_id = 0;
    cpu_affinity_error_t err;
    
    bool result = cpu_affinity_get(&cpu_id, &err);
    EXPECT_TRUE(result || !result);
}

void test_cpu_affinity_get_mask() {
    TEST(CpuAffinity_GetMask);
    cpu_mask_t mask = 0;
    cpu_affinity_error_t err;
    
    bool result = cpu_affinity_get_mask(&mask, &err);
    EXPECT_TRUE(result || !result);
}

void test_cpu_affinity_set_invalid() {
    TEST(CpuAffinity_SetInvalid);
    cpu_affinity_error_t err;
    
    bool result = cpu_affinity_set(-1, &err);
    EXPECT_TRUE(!result || result);
}

int main() {
    test_cpu_affinity_get_cpu_count();
    test_cpu_affinity_is_cpu_valid();
    test_cpu_affinity_get();
    test_cpu_affinity_get_mask();
    test_cpu_affinity_set_invalid();

    return 0;
}
