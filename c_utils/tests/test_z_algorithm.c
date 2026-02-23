#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/z_algorithm.h"

void test_z_algorithm_compute() {
    TEST(ZAlgorithm_Compute);
    const char* s = "aabcaabxaaaz";
    int z[20] = {0};
    
    z_algorithm_compute(s, z);
    
    EXPECT_TRUE(z[0] == 0);
    EXPECT_TRUE(z[1] == 1);
}

void test_z_algorithm_create_destroy() {
    TEST(ZAlgorithm_CreateDestroy);
    z_algorithm_ctx_t* ctx = NULL;
    z_algorithm_error_t err = z_algorithm_create(&ctx, NULL);
    
    EXPECT_EQ(err, Z_ALGORITHM_OK);
    EXPECT_TRUE(ctx != NULL);
    
    z_algorithm_destroy(ctx);
}

void test_z_algorithm_compute_safe() {
    TEST(ZAlgorithm_ComputeSafe);
    z_algorithm_ctx_t* ctx = NULL;
    z_algorithm_create(&ctx, NULL);
    
    const char* s = "aaaa";
    int z[10] = {0};
    z_algorithm_error_t err = z_algorithm_compute_safe(ctx, s, 4, z, 10);
    
    EXPECT_EQ(err, Z_ALGORITHM_OK);
    
    z_algorithm_destroy(ctx);
}

void test_z_algorithm_lcp() {
    TEST(ZAlgorithm_LCP);
    size_t lcp = z_algorithm_lcp("abcdef", "abcxyz");
    EXPECT_EQ(lcp, (size_t)3);
    
    lcp = z_algorithm_lcp("hello", "world");
    EXPECT_EQ(lcp, (size_t)0);
}

void test_z_algorithm_strerror() {
    TEST(ZAlgorithm_Strerror);
    const char* msg = z_algorithm_strerror(Z_ALGORITHM_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = z_algorithm_strerror(Z_ALGORITHM_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_z_algorithm_compute();
    test_z_algorithm_create_destroy();
    test_z_algorithm_compute_safe();
    test_z_algorithm_lcp();
    test_z_algorithm_strerror();

    return 0;
}
