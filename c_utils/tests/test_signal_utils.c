#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../c_utils/utest.h"
#include "../c_utils/signal_utils.h"

static int test_handler_called = 0;
static int test_handler_signum = 0;

static void test_handler(int signum, siginfo_t *info, void *context) {
    (void)info;
    (void)context;
    test_handler_called = 1;
    test_handler_signum = signum;
}

void test_signal_config_init() {
    TEST(Signal_ConfigInit);
    signal_config_t config;
    signal_config_init(&config);
    EXPECT_TRUE(config.catch_sigint);
    EXPECT_TRUE(config.catch_sigterm);
    EXPECT_FALSE(config.catch_sigquit);
    EXPECT_FALSE(config.catch_sighup);
    EXPECT_FALSE(config.catch_sigpipe);
    EXPECT_FALSE(config.catch_sigsegv);
    EXPECT_TRUE(config.use_async_safe);
    EXPECT_FALSE(config.enable_nested_handlers);
    EXPECT_EQ(config.max_handlers, (size_t)32);
}

void test_signal_state_init() {
    TEST(Signal_StateInit);
    signal_state_t state;
    signal_state_init(&state);
    EXPECT_EQ(state.last_error, SIGNAL_OK);
    EXPECT_EQ(state.signal_count, (uint32_t)0);
    EXPECT_FALSE(state.exit_requested);
    EXPECT_TRUE(state.is_initialized);
}

void test_signal_init_exit_handler() {
    TEST(Signal_InitExitHandler);
    signal_init_exit_handler();
    bool exit_requested = signal_is_exit_requested();
    EXPECT_FALSE(exit_requested);
}

void test_signal_init_ex() {
    TEST(Signal_InitEx);
    signal_config_t config;
    signal_config_init(&config);
    signal_state_t state;
    signal_error_t err = signal_init_ex(&config, &state);
    EXPECT_EQ(err, SIGNAL_OK);
    EXPECT_TRUE(state.is_initialized);
}

void test_signal_get_exit_status() {
    TEST(Signal_GetExitStatus);
    bool exit_requested;
    signal_state_t state;
    signal_error_t err = signal_get_exit_status(&exit_requested, &state);
    EXPECT_EQ(err, SIGNAL_OK);
}

void test_signal_get_exit_status_null() {
    TEST(Signal_GetExitStatusNull);
    signal_error_t err = signal_get_exit_status(NULL, NULL);
    EXPECT_EQ(err, SIGNAL_ERROR_INVALID_PARAMS);
}

void test_signal_register_handler_null() {
    TEST(Signal_RegisterHandlerNull);
    signal_state_t state;
    signal_error_t err = signal_register_handler(SIGINT, NULL, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_HANDLER_NULL);
}

void test_signal_register_handler_invalid_signal() {
    TEST(Signal_RegisterHandlerInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_register_handler(-1, test_handler, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_remove_handler_invalid_signal() {
    TEST(Signal_RemoveHandlerInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_remove_handler(-1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_block_invalid_signal() {
    TEST(Signal_BlockInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_block(-1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_unblock_invalid_signal() {
    TEST(Signal_UnblockInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_unblock(-1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_ignore_invalid_signal() {
    TEST(Signal_IgnoreInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_ignore(-1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_restore_default_invalid_signal() {
    TEST(Signal_RestoreDefaultInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_restore_default(-1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_send_invalid_signal() {
    TEST(Signal_SendInvalidSignal);
    signal_state_t state;
    signal_error_t err = signal_send(getpid(), -1, &state);
    EXPECT_EQ(err, SIGNAL_ERROR_SIGNAL_INVALID);
}

void test_signal_get_name() {
    TEST(Signal_GetName);
    const char *name = signal_get_name(SIGINT);
    EXPECT_STREQ(name, "SIGINT");
    name = signal_get_name(SIGTERM);
    EXPECT_STREQ(name, "SIGTERM");
    name = signal_get_name(9999);
    EXPECT_STREQ(name, "UNKNOWN");
}

void test_signal_get_description() {
    TEST(Signal_GetDescription);
    const char *desc = signal_get_description(SIGINT);
    EXPECT_STREQ(desc, "Interrupt from keyboard");
    desc = signal_get_description(SIGTERM);
    EXPECT_STREQ(desc, "Termination signal");
    desc = signal_get_description(9999);
    EXPECT_STREQ(desc, "Unknown signal");
}

void test_signal_reset() {
    TEST(Signal_Reset);
    signal_state_t state;
    signal_error_t err = signal_reset(&state);
    EXPECT_EQ(err, SIGNAL_OK);
    EXPECT_FALSE(state.exit_requested);
}

void test_signal_strerror() {
    TEST(Signal_Strerror);
    signal_state_t state;
    signal_state_init(&state);
    const char *err_msg = signal_strerror(&state);
    EXPECT_STREQ(err_msg, "Success");
    err_msg = signal_strerror(NULL);
    EXPECT_STREQ(err_msg, "Invalid state");
}

void test_signal_ignore() {
    TEST(Signal_Ignore);
    signal_state_t state;
    signal_error_t err = signal_ignore(SIGUSR1, &state);
    EXPECT_EQ(err, SIGNAL_OK);
}

void test_signal_restore_default() {
    TEST(Signal_RestoreDefault);
    signal_state_t state;
    signal_error_t err = signal_restore_default(SIGUSR1, &state);
    EXPECT_EQ(err, SIGNAL_OK);
}

void test_signal_block_unblock() {
    TEST(Signal_BlockUnblock);
    signal_state_t state;
    signal_error_t err = signal_block(SIGUSR2, &state);
    EXPECT_EQ(err, SIGNAL_OK);
    err = signal_unblock(SIGUSR2, &state);
    EXPECT_EQ(err, SIGNAL_OK);
}

int main() {
    test_signal_config_init();
    test_signal_state_init();
    test_signal_init_exit_handler();
    test_signal_init_ex();
    test_signal_get_exit_status();
    test_signal_get_exit_status_null();
    test_signal_register_handler_null();
    test_signal_register_handler_invalid_signal();
    test_signal_remove_handler_invalid_signal();
    test_signal_block_invalid_signal();
    test_signal_unblock_invalid_signal();
    test_signal_ignore_invalid_signal();
    test_signal_restore_default_invalid_signal();
    test_signal_send_invalid_signal();
    test_signal_get_name();
    test_signal_get_description();
    test_signal_reset();
    test_signal_strerror();
    test_signal_ignore();
    test_signal_restore_default();
    test_signal_block_unblock();

    return 0;
}
