#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/utest.h"
#include "../c_utils/timer.h"

static int callback_count = 0;

static void test_callback(void *data) {
    callback_count++;
    (void)data;
}

void test_timer_set() {
    TEST(Timer_Set);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    
    timer_set(&t, 100, test_callback, NULL);
    
    EXPECT_TRUE(t.active);
    EXPECT_TRUE(t.cb == test_callback);
    
    timer_cancel(&t, NULL);
}

void test_timer_update() {
    TEST(Timer_Update);
    cutils_timer_t t;
    memset(&t, 0, sizeof(t));
    callback_count = 0;
    
    timer_set(&t, 50, test_callback, NULL);
    
    usleep(60000);
    
    timer_update(&t);
    
    EXPECT_EQ(callback_count, 1);
    EXPECT_FALSE(t.active);
}

void test_timer_cancel() {
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

void test_timer_is_active() {
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

void test_timer_get_remaining() {
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

int main() {
    test_timer_set();
    test_timer_update();
    test_timer_cancel();
    test_timer_is_active();
    test_timer_get_remaining();

    return 0;
}
