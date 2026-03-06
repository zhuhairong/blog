#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../c_utils/utest.h"
#include "../c_utils/timer.h"

static int callback_count = 0;
static void *last_callback_data = NULL;
static uint32_t last_delay_ms = 0;

static void test_callback(void *data) {
    callback_count++;
    last_callback_data = data;
}

static void test_callback_with_delay(void *data) {
    callback_count++;
    if (data != NULL) {
        last_delay_ms = *(uint32_t *)data;
    }
}

static uint64_t get_current_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* ==================== 基础测试 ==================== */

void test_timer_set(void) {
    TEST(Timer_Set);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    uint32_t test_data = 42;
    
    timer_set(&t, 100, test_callback, &test_data);
    
    EXPECT_TRUE(t.active);
    EXPECT_TRUE(t.cb == test_callback);
    EXPECT_EQ(t.data, &test_data);
    EXPECT_TRUE(t.target_ms > 0);
    EXPECT_TRUE(t.target_ms > get_current_ms());
    
    timer_cancel(&t, NULL);
}

void test_timer_update(void) {
    TEST(Timer_Update);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    callback_count = 0;
    last_callback_data = NULL;
    uint32_t test_data = 123;
    
    timer_set(&t, 50, test_callback, &test_data);
    
    usleep(60000);
    
    timer_update(&t);
    
    EXPECT_EQ(callback_count, 1);
    EXPECT_FALSE(t.active);
    EXPECT_EQ(last_callback_data, &test_data);
}

void test_timer_cancel(void) {
    TEST(Timer_Cancel);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    callback_count = 0;
    
    timer_set(&t, 1000, test_callback, NULL);
    
    timer_error_t error = timer_cancel(&t, NULL);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_FALSE(t.active);
    
    usleep(100000);
    timer_update(&t);
    
    EXPECT_EQ(callback_count, 0);
}

void test_timer_is_active(void) {
    TEST(Timer_IsActive);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    
    bool active;
    timer_is_active(&t, &active);
    EXPECT_FALSE(active);
    
    timer_set(&t, 1000, test_callback, NULL);
    
    timer_is_active(&t, &active);
    EXPECT_TRUE(active);
    
    timer_cancel(&t, NULL);
}

void test_timer_get_remaining(void) {
    TEST(Timer_GetRemaining);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    
    timer_set(&t, 1000, test_callback, NULL);
    
    uint32_t remaining;
    timer_error_t error = timer_get_remaining(&t, &remaining, NULL);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_TRUE(remaining > 0);
    EXPECT_TRUE(remaining <= 1000);
    
    timer_cancel(&t, NULL);
}

/* ==================== 参数验证测试 ==================== */

void test_timer_set_ex_null_timer(void) {
    TEST(Timer_Set_Ex_Null_Timer);
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    
    timer_error_t error = timer_set_ex(NULL, 100, test_callback, NULL, &config, &state);
    EXPECT_EQ(error, TIMER_ERROR_TIMER_NULL);
}

void test_timer_set_ex_null_callback(void) {
    TEST(Timer_Set_Ex_Null_Callback);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    
    timer_error_t error = timer_set_ex(&t, 100, NULL, NULL, &config, &state);
    EXPECT_EQ(error, TIMER_ERROR_CALLBACK_NULL);
}

void test_timer_set_ex_zero_delay(void) {
    TEST(Timer_Set_Ex_Zero_Delay);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    
    timer_error_t error = timer_set_ex(&t, 0, test_callback, NULL, &config, &state);
    EXPECT_EQ(error, TIMER_ERROR_DELAY_ZERO);
}

void test_timer_cancel_null_timer(void) {
    TEST(Timer_Cancel_Null_Timer);
    timer_error_t error = timer_cancel(NULL, NULL);
    EXPECT_EQ(error, TIMER_ERROR_TIMER_NULL);
}

void test_timer_cancel_inactive_timer(void) {
    TEST(Timer_Cancel_Inactive_Timer);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_state_t state;
    timer_state_init(&state);
    
    timer_error_t error = timer_cancel(&t, &state);
    EXPECT_EQ(error, TIMER_ERROR_NOT_ACTIVE);
}

void test_timer_get_remaining_null_params(void) {
    TEST(Timer_Get_Remaining_Null_Params);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_set(&t, 1000, test_callback, NULL);
    
    timer_error_t error = timer_get_remaining(NULL, NULL, NULL);
    EXPECT_EQ(error, TIMER_ERROR_INVALID_PARAMS);
    
    uint32_t remaining;
    error = timer_get_remaining(&t, NULL, NULL);
    EXPECT_EQ(error, TIMER_ERROR_INVALID_PARAMS);
    
    error = timer_get_remaining(NULL, &remaining, NULL);
    EXPECT_EQ(error, TIMER_ERROR_INVALID_PARAMS);
    
    timer_cancel(&t, NULL);
}

void test_timer_get_remaining_inactive_timer(void) {
    TEST(Timer_Get_Remaining_Inactive_Timer);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_state_t state;
    timer_state_init(&state);
    
    uint32_t remaining;
    timer_error_t error = timer_get_remaining(&t, &remaining, &state);
    EXPECT_EQ(error, TIMER_ERROR_NOT_ACTIVE);
}

/* ==================== 边界情况测试 ==================== */

void test_timer_zero_delay(void) {
    TEST(Timer_Zero_Delay);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    callback_count = 0;
    
    timer_set(&t, 0, test_callback, NULL);
    
    if (t.active) {
        timer_update(&t);
        EXPECT_EQ(callback_count, 1);
        timer_cancel(&t, NULL);
    } else {
        EXPECT_FALSE(t.active);
    }
}

void test_timer_max_delay(void) {
    TEST(Timer_Max_Delay);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    
    timer_set(&t, UINT32_MAX, test_callback, NULL);
    
    EXPECT_TRUE(t.active);
    EXPECT_TRUE(t.target_ms > get_current_ms());
    
    uint32_t remaining;
    timer_get_remaining(&t, &remaining, NULL);
    EXPECT_TRUE(remaining > 0);
    
    timer_cancel(&t, NULL);
}

void test_timer_multiple_updates(void) {
    TEST(Timer_Multiple_Updates);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    callback_count = 0;
    
    timer_set(&t, 50, test_callback, NULL);
    
    for (int i = 0; i < 10; i++) {
        timer_update(&t);
        usleep(10000);
    }
    
    EXPECT_EQ(callback_count, 1);
    EXPECT_FALSE(t.active);
}

/* ==================== 扩展功能测试 ==================== */

void test_timer_set_ex_basic(void) {
    TEST(Timer_Set_Ex_Basic);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    uint32_t test_data = 999;
    
    timer_error_t error = timer_set_ex(&t, 100, test_callback, &test_data, &config, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_TRUE(t.active);
    EXPECT_TRUE(t.cb == test_callback);
    EXPECT_EQ(t.data, &test_data);
    EXPECT_TRUE(t.target_ms > 0);
    
    timer_cancel(&t, NULL);
}

void test_timer_update_ex_basic(void) {
    TEST(Timer_Update_Ex_Basic);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    callback_count = 0;
    
    timer_set_ex(&t, 50, test_callback, NULL, &config, &state);
    
    usleep(60000);
    
    timer_error_t error = timer_update_ex(&t, &config, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_EQ(callback_count, 1);
    EXPECT_FALSE(t.active);
}

void test_timer_repeating_basic(void) {
    TEST(Timer_Repeating_Basic);
    timer_extended_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    callback_count = 0;
    
    timer_error_t error = timer_set_repeating(&t, 30, 30, 3, test_callback, NULL, &config, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_TRUE(t.active);
    EXPECT_TRUE(t.repeating);
    EXPECT_EQ(t.repeat_interval, 30);
    EXPECT_EQ(t.max_repeats, 3);
    
    for (int i = 0; i < 5; i++) {
        usleep(40000);
        timer_update_extended(&t, &config, &state);
    }
    
    EXPECT_EQ(callback_count, 3);
    EXPECT_FALSE(t.active);
}

void test_timer_repeating_max_repeats(void) {
    TEST(Timer_Repeating_Max_Repeats);
    timer_extended_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    callback_count = 0;
    
    timer_error_t error = timer_set_repeating(&t, 20, 20, 5, test_callback, NULL, &config, &state);
    EXPECT_EQ(error, TIMER_OK);
    
    for (int i = 0; i < 8; i++) {
        usleep(25000);
        timer_update_extended(&t, &config, &state);
    }
    
    EXPECT_EQ(callback_count, 5);
    EXPECT_EQ(t.repeat_count, 5);
    EXPECT_FALSE(t.active);
}

void test_timer_extended_cancel(void) {
    TEST(Timer_Extended_Cancel);
    timer_extended_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    callback_count = 0;
    
    timer_set_repeating(&t, 1000, 100, 10, test_callback, NULL, &config, &state);
    
    timer_error_t error = timer_cancel_extended(&t, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_FALSE(t.active);
    
    usleep(150000);
    timer_update_extended(&t, &config, &state);
    
    EXPECT_EQ(callback_count, 0);
}

void test_timer_strerror(void) {
    TEST(Timer_Strerror);
    timer_state_t state;
    timer_state_init(&state);
    
    state.last_error = TIMER_OK;
    const char *msg = timer_strerror(&state);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    state.last_error = TIMER_ERROR_TIMER_NULL;
    msg = timer_strerror(&state);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
    
    state.last_error = TIMER_ERROR_CALLBACK_NULL;
    msg = timer_strerror(&state);
    EXPECT_TRUE(msg != NULL);
    EXPECT_TRUE(strlen(msg) > 0);
}

void test_timer_config_init(void) {
    TEST(Timer_Config_Init);
    timer_config_t config;
    memset(&config, 0xFF, sizeof(config));
    
    timer_config_init(&config);
    
    EXPECT_FALSE(config.enable_statistics);
    EXPECT_TRUE(config.enable_validation);
    EXPECT_FALSE(config.enable_thread_safety);
    EXPECT_FALSE(config.auto_cleanup);
    EXPECT_EQ(config.max_timers, 1024);
    EXPECT_EQ(config.buffer_size, 4096);
    EXPECT_EQ(config.max_delay, 86400000ULL);
    EXPECT_FALSE(config.use_high_resolution);
}

void test_timer_state_init(void) {
    TEST(Timer_State_Init);
    timer_state_t state;
    memset(&state, 0xFF, sizeof(state));
    
    timer_state_init(&state);
    
    EXPECT_EQ(state.last_error, TIMER_OK);
    EXPECT_EQ(state.timer_sets, 0);
    EXPECT_EQ(state.timer_triggers, 0);
    EXPECT_EQ(state.timer_cancels, 0);
    EXPECT_TRUE(state.is_initialized);
    EXPECT_EQ(state.active_timers, 0);
    EXPECT_EQ(state.average_delay, 0);
}

void test_timer_validate(void) {
    TEST(Timer_Validate);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_state_t state;
    timer_state_init(&state);
    bool valid = false;
    
    timer_error_t error = timer_validate(NULL, &valid, &state);
    EXPECT_EQ(error, TIMER_ERROR_INVALID_PARAMS);
    
    timer_set(&t, 100, test_callback, NULL);
    
    error = timer_validate(&t, &valid, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_TRUE(valid);
    
    timer_cancel(&t, NULL);
    
    error = timer_validate(&t, &valid, &state);
    EXPECT_EQ(error, TIMER_OK);
    EXPECT_TRUE(valid);
}

void test_timer_get_statistics(void) {
    TEST(Timer_Get_Statistics);
    timer_state_t state;
    timer_state_init(&state);
    
    timer_error_t error = timer_get_statistics(&state);
    EXPECT_EQ(error, TIMER_OK);
}

void test_timer_batch_operations(void) {
    TEST(Timer_Batch_Operations);
    cutils_timer_t timers[3];
    memset(timers, 0, sizeof(timers));
    uint32_t delays[3] = {100, 200, 300};
    timer_callback_t cbs[3] = {test_callback, test_callback, test_callback};
    void *datas[3] = {NULL, NULL, NULL};
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    
    timer_error_t error = timer_batch_set(timers, 3, delays, cbs, datas, &config, &state);
    EXPECT_EQ(error, TIMER_OK);
    
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(timers[i].active);
    }
    
    error = timer_batch_cancel(timers, 3, &state);
    EXPECT_EQ(error, TIMER_OK);
    
    for (int i = 0; i < 3; i++) {
        EXPECT_FALSE(timers[i].active);
    }
}

/* ==================== 状态跟踪测试 ==================== */

void test_timer_state_tracking(void) {
    TEST(Timer_State_Tracking);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    timer_config_t config;
    timer_state_t state;
    timer_config_init(&config);
    timer_state_init(&state);
    callback_count = 0;
    
    size_t initial_sets = state.timer_sets;
    size_t initial_triggers = state.timer_triggers;
    size_t initial_cancels = state.timer_cancels;
    
    timer_set_ex(&t, 50, test_callback, NULL, &config, &state);
    EXPECT_TRUE(state.timer_sets >= initial_sets + 1);
    
    usleep(60000);
    timer_update_ex(&t, &config, &state);
    EXPECT_TRUE(state.timer_triggers >= initial_triggers + 1);
    
    timer_set_ex(&t, 1000, test_callback, NULL, &config, &state);
    timer_cancel(&t, &state);
    EXPECT_TRUE(state.timer_cancels >= initial_cancels + 1);
}

/* ==================== 主函数 ==================== */

int main(void) {
    printf("\n========== 基础测试 ==========\n");
    test_timer_set();
    test_timer_update();
    test_timer_cancel();
    test_timer_is_active();
    test_timer_get_remaining();
    
    printf("\n========== 参数验证测试 ==========\n");
    test_timer_set_ex_null_timer();
    test_timer_set_ex_null_callback();
    test_timer_set_ex_zero_delay();
    test_timer_cancel_null_timer();
    test_timer_cancel_inactive_timer();
    test_timer_get_remaining_null_params();
    test_timer_get_remaining_inactive_timer();
    
    printf("\n========== 边界情况测试 ==========\n");
    test_timer_zero_delay();
    test_timer_max_delay();
    test_timer_multiple_updates();
    
    printf("\n========== 扩展功能测试 ==========\n");
    test_timer_set_ex_basic();
    test_timer_update_ex_basic();
    test_timer_repeating_basic();
    test_timer_repeating_max_repeats();
    test_timer_extended_cancel();
    test_timer_strerror();
    test_timer_config_init();
    test_timer_state_init();
    test_timer_validate();
    test_timer_get_statistics();
    test_timer_batch_operations();
    
    printf("\n========== 状态跟踪测试 ==========\n");
    test_timer_state_tracking();
    
    printf("\n========== 测试完成 ==========\n");
    
    return 0;
}
