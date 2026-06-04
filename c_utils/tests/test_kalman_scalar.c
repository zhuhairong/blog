#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/kalman_scalar.h"

void test_kalman_scalar_init() {
    TEST(KalmanScalar_Init);
    kalman_scalar_t kf;
    memset(&kf, 0, sizeof(kf));
    
    kalman_scalar_error_t err = kalman_scalar_init(&kf, 0.0, 1.0, 0.1, 0.5);
    EXPECT_EQ(err, KALMAN_SCALAR_OK);
}

void test_kalman_scalar_update() {
    TEST(KalmanScalar_Update);
    kalman_scalar_t kf;
    memset(&kf, 0, sizeof(kf));
    
    kalman_scalar_init(&kf, 0.0, 1.0, 0.1, 0.5);
    
    double estimate = kalman_scalar_update(&kf, 1.0);
    EXPECT_TRUE(isfinite(estimate)); /* kalman_scalar_update produces a finite output */
}

void test_kalman_scalar_predict() {
    TEST(KalmanScalar_Predict);
    kalman_scalar_t kf;
    memset(&kf, 0, sizeof(kf));
    
    kalman_scalar_init(&kf, 0.0, 1.0, 0.1, 0.5);
    
    double estimate = kalman_scalar_predict(&kf);
    EXPECT_TRUE(isfinite(estimate)); /* kalman_scalar_predict produces a finite output */
}

void test_kalman_scalar_types() {
    TEST(KalmanScalar_Types);
    kalman_scalar_config_t config;
    memset(&config, 0, sizeof(config));
    /* kalman_scalar_config_t type compiles and is instantiable */

    kalman_scalar_t kf;
    memset(&kf, 0, sizeof(kf));
    /* kalman_scalar_t type compiles and is instantiable */
}

void test_kalman_scalar_error_values() {
    TEST(KalmanScalar_ErrorValues);
    EXPECT_TRUE(KALMAN_SCALAR_OK == 0);
    EXPECT_TRUE(KALMAN_SCALAR_INVALID_INPUT != 0);
}

int main() {
    UTEST_BEGIN();
    test_kalman_scalar_init();
    test_kalman_scalar_update();
    test_kalman_scalar_predict();
    test_kalman_scalar_types();
    test_kalman_scalar_error_values();

    UTEST_END();
}
