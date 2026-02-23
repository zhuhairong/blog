#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/time_utils.h"

void test_time_now_ms() {
    TEST(Time_NowMs);
    uint64_t t1 = time_now_ms();
    EXPECT_TRUE(t1 > 0);
    
    time_sleep_ms(10);
    
    uint64_t t2 = time_now_ms();
    EXPECT_TRUE(t2 > t1);
}

void test_time_now_us() {
    TEST(Time_NowUs);
    uint64_t t1 = time_now_us();
    EXPECT_TRUE(t1 > 0);
}

void test_time_sleep_ms() {
    TEST(Time_SleepMs);
    uint64_t start = time_now_ms();
    
    time_sleep_ms(50);
    
    uint64_t elapsed = time_now_ms() - start;
    EXPECT_TRUE(elapsed >= 45);
    EXPECT_TRUE(elapsed < 100);
}

void test_time_format_now() {
    TEST(Time_FormatNow);
    char buf[64];
    time_format_now(buf, sizeof(buf));
    
    EXPECT_TRUE(strlen(buf) > 0);
}

void test_time_format_now_null_buf() {
    TEST(Time_FormatNowNullBuf);
    time_format_now(NULL, 0);
}

int main() {
    test_time_now_ms();
    test_time_now_us();
    test_time_sleep_ms();
    test_time_format_now();
    test_time_format_now_null_buf();

    return 0;
}
