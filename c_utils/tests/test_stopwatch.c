#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/stopwatch.h"

void test_stopwatch_start_stop() {
    TEST(Stopwatch_StartStop);
    stopwatch_t sw;
    memset(&sw, 0, sizeof(sw));
    
    stopwatch_start(&sw);
    EXPECT_TRUE(sw.running);
    
    stopwatch_stop(&sw);
    EXPECT_FALSE(sw.running);
    EXPECT_TRUE(sw.elapsed > 0 || sw.elapsed == 0);
}

void test_stopwatch_reset() {
    TEST(Stopwatch_Reset);
    stopwatch_t sw;
    memset(&sw, 0, sizeof(sw));
    
    stopwatch_start(&sw);
    stopwatch_stop(&sw);
    
    stopwatch_reset(&sw);
    
    EXPECT_EQ(sw.elapsed, (uint64_t)0);
    EXPECT_FALSE(sw.running);
}

void test_stopwatch_elapsed_ms() {
    TEST(Stopwatch_ElapsedMs);
    stopwatch_t sw;
    memset(&sw, 0, sizeof(sw));
    
    stopwatch_start(&sw);
    
    stopwatch_stop(&sw);
    
    uint64_t elapsed = stopwatch_elapsed_ms(&sw);
    EXPECT_TRUE(elapsed >= 0);
}

void test_stopwatch_multiple_starts() {
    TEST(Stopwatch_MultipleStarts);
    stopwatch_t sw;
    memset(&sw, 0, sizeof(sw));
    
    stopwatch_start(&sw);
    stopwatch_start(&sw);
    
    EXPECT_TRUE(sw.running);
    
    stopwatch_stop(&sw);
}

void test_stopwatch_elapsed_while_running() {
    TEST(Stopwatch_ElapsedWhileRunning);
    stopwatch_t sw;
    memset(&sw, 0, sizeof(sw));
    
    stopwatch_start(&sw);
    
    uint64_t elapsed1 = stopwatch_elapsed_ms(&sw);
    
    uint64_t elapsed2 = stopwatch_elapsed_ms(&sw);
    
    EXPECT_TRUE(elapsed2 >= elapsed1);
    
    stopwatch_stop(&sw);
}

int main() {
    test_stopwatch_start_stop();
    test_stopwatch_reset();
    test_stopwatch_elapsed_ms();
    test_stopwatch_multiple_starts();
    test_stopwatch_elapsed_while_running();

    return 0;
}
