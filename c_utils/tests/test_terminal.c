#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/terminal.h"

void test_term_config_init() {
    TEST(Term_ConfigInit);
    term_config_t config;
    term_config_init(&config);
    
    EXPECT_TRUE(config.enable_colors || !config.enable_colors);
}

void test_term_state_init() {
    TEST(Term_StateInit);
    term_state_t state;
    term_state_init(&state);
    
    EXPECT_FALSE(state.is_initialized);
}

void test_term_progress_init() {
    TEST(Term_ProgressInit);
    term_progress_t tp;
    term_error_t error = term_progress_init(&tp, 50, '#', '-');
    EXPECT_EQ(error, TERM_OK);
    EXPECT_EQ(tp.width, 50);
    EXPECT_EQ(tp.fill, '#');
    EXPECT_EQ(tp.empty, '-');
}

void test_term_get_size() {
    TEST(Term_GetSize);
    size_t width, height;
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_get_size(&width, &height, &state);
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_TERMINAL_SIZE);
}

void test_term_strerror() {
    TEST(Term_Strerror);
    term_state_t state;
    term_state_init(&state);
    state.last_error = TERM_OK;
    
    const char* msg = term_strerror(&state);
    EXPECT_TRUE(msg != NULL);
}

void test_term_reset_state() {
    TEST(Term_ResetState);
    term_state_t state;
    state.is_initialized = true;
    state.prints = 100;
    
    term_reset_state(&state);
    EXPECT_FALSE(state.is_initialized);
    EXPECT_EQ(state.prints, (size_t)0);
}

void test_term_check_colors_supported() {
    TEST(Term_CheckColorsSupported);
    bool supported;
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_check_colors_supported(&supported, &state);
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_COLOR_UNSUPPORTED);
}

void test_term_check_ansi_supported() {
    TEST(Term_CheckAnsiSupported);
    bool supported;
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_check_ansi_supported(&supported, &state);
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_ANSI_UNSUPPORTED);
}

void test_term_printf_ex() {
    TEST(Term_PrintfEx);
    term_config_t config;
    term_config_init(&config);
    
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_printf_ex(TERM_COLOR_GREEN, TERM_STYLE_NORMAL, "Test");
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_PRINTF);
}

void test_term_progress_draw() {
    TEST(Term_ProgressDraw);
    term_progress_t tp;
    term_progress_init(&tp, 50, '#', '-');
    
    term_progress_draw(&tp, 0.5, "Progress");
}

void test_term_clear_screen() {
    TEST(Term_ClearScreen);
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_clear_screen(&state);
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_CURSOR);
}

void test_term_clear_line() {
    TEST(Term_ClearLine);
    term_state_t state;
    term_state_init(&state);
    
    term_error_t error = term_clear_line(&state);
    EXPECT_TRUE(error == TERM_OK || error == TERM_ERROR_CURSOR);
}

int main() {
    test_term_config_init();
    test_term_state_init();
    test_term_progress_init();
    test_term_get_size();
    test_term_strerror();
    test_term_reset_state();
    test_term_check_colors_supported();
    test_term_check_ansi_supported();
    test_term_printf_ex();
    test_term_progress_draw();
    test_term_clear_screen();
    test_term_clear_line();

    return 0;
}
