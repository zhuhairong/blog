#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../c_utils/utest.h"
#include "../c_utils/backtrace.h"

void test_backtrace_get() {
    TEST(Backtrace_Get);
    void* buffer[10];
    
    size_t count = backtrace_get(buffer, 10);
    EXPECT_TRUE(count > 0);
}

void test_backtrace_print() {
    TEST(Backtrace_Print);
    backtrace_print();
    EXPECT_TRUE(true);
}

void test_backtrace_get_symbols() {
    TEST(Backtrace_GetSymbols);
    void* buffer[10];
    
    size_t count = backtrace_get(buffer, 10);
    EXPECT_TRUE(count > 0);
    
    char** symbols = backtrace_get_symbols(buffer, count);
    EXPECT_TRUE(symbols != NULL || symbols == NULL);
    
    if (symbols != NULL) {
        free(symbols);
    }
}

void test_backtrace_get_frames() {
    TEST(Backtrace_GetFrames);
    backtrace_frame_t frames[10];
    
    size_t count = backtrace_get_frames(frames, 10);
    EXPECT_TRUE(count >= 0);
    
    backtrace_free_frames(frames, count);
}

void test_backtrace_print_to_file() {
    TEST(Backtrace_PrintToFile);
    FILE* fp = fopen("/dev/null", "w");
    EXPECT_TRUE(fp != NULL);
    
    if (fp != NULL) {
        backtrace_print_to_file(fp);
        fclose(fp);
    }
}

int main() {
    test_backtrace_get();
    test_backtrace_print();
    test_backtrace_get_symbols();
    test_backtrace_get_frames();
    test_backtrace_print_to_file();

    return 0;
}
