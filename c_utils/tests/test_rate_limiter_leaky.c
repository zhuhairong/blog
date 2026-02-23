#include <stdio.h>
#include "../c_utils/utest.h"
#include "../c_utils/rate_limiter_leaky.h"

void test_leaky_bucket_default_config() {
    TEST(LeakyBucket_DefaultConfig);
    leaky_bucket_config_t config = leaky_bucket_default_config();
    EXPECT_EQ(config.capacity, 100.0);
    EXPECT_EQ(config.rate, 10.0);
    EXPECT_EQ(config.initial_water, 0.0);
    EXPECT_TRUE(config.use_monotonic_time);
    EXPECT_TRUE(config.allow_burst);
}

void test_leaky_init() {
    TEST(Leaky_Init);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    EXPECT_EQ(lb.capacity, 100.0);
    EXPECT_EQ(lb.rate, 10.0);
    EXPECT_EQ(lb.water, 0.0);
}

void test_leaky_init_ex() {
    TEST(Leaky_InitEx);
    leaky_bucket_t lb;
    leaky_bucket_config_t config = leaky_bucket_default_config();
    leaky_bucket_error_t error;
    bool success = leaky_init_ex(&lb, &config, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
    EXPECT_EQ(lb.capacity, 100.0);
}

void test_leaky_init_ex_null() {
    TEST(Leaky_InitExNull);
    leaky_bucket_error_t error;
    bool success = leaky_init_ex(NULL, NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_init_ex_invalid_capacity() {
    TEST(Leaky_InitExInvalidCapacity);
    leaky_bucket_t lb;
    leaky_bucket_config_t config = leaky_bucket_default_config();
    config.capacity = 0.0;
    leaky_bucket_error_t error;
    bool success = leaky_init_ex(&lb, &config, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL);
}

void test_leaky_init_ex_invalid_rate() {
    TEST(Leaky_InitExInvalidRate);
    leaky_bucket_t lb;
    leaky_bucket_config_t config = leaky_bucket_default_config();
    config.rate = 0.0;
    leaky_bucket_error_t error;
    bool success = leaky_init_ex(&lb, &config, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_RATE_TOO_SLOW);
}

void test_leaky_consume() {
    TEST(Leaky_Consume);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    bool success = leaky_consume(&lb, 50.0);
    EXPECT_TRUE(success);
    EXPECT_EQ(lb.water, 50.0);
}

void test_leaky_consume_ex() {
    TEST(Leaky_ConsumeEx);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool success = leaky_consume_ex(&lb, 50.0, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
    EXPECT_EQ(lb.water, 50.0);
}

void test_leaky_consume_ex_null() {
    TEST(Leaky_ConsumeExNull);
    leaky_bucket_error_t error;
    bool success = leaky_consume_ex(NULL, 50.0, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_consume_ex_invalid_amount() {
    TEST(Leaky_ConsumeExInvalidAmount);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool success = leaky_consume_ex(&lb, -1.0, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_INVALID_ARGS);
}

void test_leaky_consume_ex_amount_too_large() {
    TEST(Leaky_ConsumeExAmountTooLarge);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool success = leaky_consume_ex(&lb, 200.0, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE);
}

void test_leaky_get_state() {
    TEST(Leaky_GetState);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_state_t state;
    leaky_bucket_error_t error;
    bool success = leaky_get_state(&lb, &state, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
    EXPECT_EQ(state.current_water, 0.0);
    EXPECT_EQ(state.available_capacity, 100.0);
    EXPECT_TRUE(state.is_empty);
    EXPECT_FALSE(state.is_full);
}

void test_leaky_get_state_null() {
    TEST(Leaky_GetStateNull);
    leaky_bucket_state_t state;
    leaky_bucket_error_t error;
    bool success = leaky_get_state(NULL, &state, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_reset() {
    TEST(Leaky_Reset);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_consume(&lb, 50.0);
    leaky_bucket_error_t error;
    bool success = leaky_reset(&lb, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
    EXPECT_EQ(lb.water, 0.0);
}

void test_leaky_reset_null() {
    TEST(Leaky_ResetNull);
    leaky_bucket_error_t error;
    bool success = leaky_reset(NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_update() {
    TEST(Leaky_Update);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool success = leaky_update(&lb, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
}

void test_leaky_update_null() {
    TEST(Leaky_UpdateNull);
    leaky_bucket_error_t error;
    bool success = leaky_update(NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_calculate_wait_time() {
    TEST(Leaky_CalculateWaitTime);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_consume(&lb, 80.0);
    int wait_ms;
    leaky_bucket_error_t error;
    bool success = leaky_calculate_wait_time(&lb, 50.0, &wait_ms, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
}

void test_leaky_calculate_wait_time_null() {
    TEST(Leaky_CalculateWaitTimeNull);
    int wait_ms;
    leaky_bucket_error_t error;
    bool success = leaky_calculate_wait_time(NULL, 50.0, &wait_ms, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_set_params() {
    TEST(Leaky_SetParams);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool success = leaky_set_params(&lb, 200.0, 20.0, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
    EXPECT_EQ(lb.capacity, 200.0);
    EXPECT_EQ(lb.rate, 20.0);
}

void test_leaky_set_params_null() {
    TEST(Leaky_SetParamsNull);
    leaky_bucket_error_t error;
    bool success = leaky_set_params(NULL, 200.0, 20.0, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_can_consume() {
    TEST(Leaky_CanConsume);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    bool can_consume = leaky_can_consume(&lb, 50.0, &error);
    EXPECT_TRUE(can_consume);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
}

void test_leaky_can_consume_null() {
    TEST(Leaky_CanConsumeNull);
    leaky_bucket_error_t error;
    bool can_consume = leaky_can_consume(NULL, 50.0, &error);
    EXPECT_FALSE(can_consume);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_get_available() {
    TEST(Leaky_GetAvailable);
    leaky_bucket_t lb;
    leaky_init(&lb, 100.0, 10.0);
    leaky_bucket_error_t error;
    double available = leaky_get_available(&lb, &error);
    EXPECT_EQ(available, 100.0);
    EXPECT_EQ(error, LEAKY_BUCKET_OK);
}

void test_leaky_get_available_null() {
    TEST(Leaky_GetAvailableNull);
    leaky_bucket_error_t error;
    double available = leaky_get_available(NULL, &error);
    EXPECT_EQ(available, 0.0);
    EXPECT_EQ(error, LEAKY_BUCKET_ERROR_NULL_PTR);
}

void test_leaky_bucket_error_string() {
    TEST(LeakyBucket_ErrorString);
    const char* msg = leaky_bucket_error_string(LEAKY_BUCKET_OK);
    EXPECT_STREQ(msg, "Success");
    msg = leaky_bucket_error_string(LEAKY_BUCKET_ERROR_NULL_PTR);
    EXPECT_STREQ(msg, "Null pointer error");
    msg = leaky_bucket_error_string(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_leaky_bucket_default_config();
    test_leaky_init();
    test_leaky_init_ex();
    test_leaky_init_ex_null();
    test_leaky_init_ex_invalid_capacity();
    test_leaky_init_ex_invalid_rate();
    test_leaky_consume();
    test_leaky_consume_ex();
    test_leaky_consume_ex_null();
    test_leaky_consume_ex_invalid_amount();
    test_leaky_consume_ex_amount_too_large();
    test_leaky_get_state();
    test_leaky_get_state_null();
    test_leaky_reset();
    test_leaky_reset_null();
    test_leaky_update();
    test_leaky_update_null();
    test_leaky_calculate_wait_time();
    test_leaky_calculate_wait_time_null();
    test_leaky_set_params();
    test_leaky_set_params_null();
    test_leaky_can_consume();
    test_leaky_can_consume_null();
    test_leaky_get_available();
    test_leaky_get_available_null();
    test_leaky_bucket_error_string();

    return 0;
}
