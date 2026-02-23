#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "../c_utils/utest.h"
#include "../c_utils/fast_fourier_transform.h"

void test_fft_create_destroy() {
    TEST(FFT_CreateDestroy);
    fft_ctx_t* ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 65536,
        .max_batch_size = 16
    };
    
    fft_error_t err = fft_create(&ctx, &config);
    EXPECT_EQ(err, FFT_OK);
    EXPECT_TRUE(ctx != NULL);
    
    fft_destroy(ctx);
}

void test_fft_create_null_config() {
    TEST(FFT_CreateNullConfig);
    fft_ctx_t* ctx = NULL;
    
    fft_error_t err = fft_create(&ctx, NULL);
    EXPECT_EQ(err, FFT_OK);
    EXPECT_TRUE(ctx != NULL);
    
    fft_destroy(ctx);
}

void test_fft_strerror() {
    TEST(FFT_Strerror);
    const char* msg = fft_strerror(FFT_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = fft_strerror(FFT_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
    
    msg = fft_strerror(FFT_MEMORY_ERROR);
    EXPECT_TRUE(msg != NULL);
}

void test_fft_compute_basic() {
    TEST(FFT_ComputeBasic);
    fft_ctx_t* ctx = NULL;
    fft_error_t err = fft_create(&ctx, NULL);
    EXPECT_EQ(err, FFT_OK);
    
    if (ctx != NULL) {
        double _Complex data[4] = {1, 0, 0, 0};
        
        err = fft_compute_safe(ctx, data, 4);
        EXPECT_EQ(err, FFT_OK);
        
        fft_destroy(ctx);
    }
}

void test_fft_inverse_basic() {
    TEST(FFT_InverseBasic);
    fft_ctx_t* ctx = NULL;
    fft_error_t err = fft_create(&ctx, NULL);
    EXPECT_EQ(err, FFT_OK);
    
    if (ctx != NULL) {
        double _Complex data[4] = {1, 0, 0, 0};
        
        err = fft_compute_safe(ctx, data, 4);
        EXPECT_EQ(err, FFT_OK);
        
        err = fft_inverse(ctx, data, 4);
        EXPECT_EQ(err, FFT_OK);
        
        fft_destroy(ctx);
    }
}

int main() {
    test_fft_create_destroy();
    test_fft_create_null_config();
    test_fft_strerror();
    test_fft_compute_basic();
    test_fft_inverse_basic();

    return 0;
}
