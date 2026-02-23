#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/stats.h"

void test_stats_compute() {
    TEST(Stats_Compute);
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stats_t result = stats_compute(data, 5);
    
    EXPECT_TRUE(fabs(result.min - 1.0) < 0.001);
    EXPECT_TRUE(fabs(result.max - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.mean - 3.0) < 0.001);
}

void test_stats_compute_single() {
    TEST(Stats_ComputeSingle);
    double data[] = {5.0};
    
    stats_t result = stats_compute(data, 1);
    
    EXPECT_TRUE(fabs(result.min - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.max - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.mean - 5.0) < 0.001);
}

void test_stats_compute_negative() {
    TEST(Stats_ComputeNegative);
    double data[] = {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0};
    
    stats_t result = stats_compute(data, 6);
    
    EXPECT_TRUE(fabs(result.min - (-5.0)) < 0.001);
    EXPECT_TRUE(fabs(result.max - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.mean - 0.0) < 0.001);
}

void test_stats_compute_variance() {
    TEST(Stats_ComputeVariance);
    double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    
    stats_t result = stats_compute(data, 8);
    
    EXPECT_TRUE(fabs(result.mean - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.variance - 4.0) < 0.001);
    EXPECT_TRUE(fabs(result.stddev - 2.0) < 0.001);
}

void test_stats_compute_empty() {
    TEST(Stats_ComputeEmpty);
    stats_t result = stats_compute(NULL, 0);
    
    EXPECT_TRUE(isnan(result.min) || result.min == 0);
}

int main() {
    test_stats_compute();
    test_stats_compute_single();
    test_stats_compute_negative();
    test_stats_compute_variance();
    test_stats_compute_empty();

    return 0;
}
