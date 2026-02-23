#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/process.h"

void test_process_default_config() {
    TEST(Process_DefaultConfig);
    process_config_t config = process_default_config();
    
    EXPECT_TRUE(config.max_output_size >= 0);
}

void test_process_run() {
    TEST(Process_Run);
    int exit_code = process_run("echo test");
    EXPECT_EQ(exit_code, 0);
}

void test_process_exec() {
    TEST(Process_Exec);
    int exit_code;
    char* output = process_exec("echo hello", &exit_code);
    
    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(output != NULL);
    EXPECT_TRUE(strstr(output, "hello") != NULL);
    
    free(output);
}

void test_process_exec_ex() {
    TEST(Process_ExecEx);
    process_config_t config = process_default_config();
    process_result_t result;
    process_error_t error;
    
    bool success = process_exec_ex("echo world", &config, &result, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_TRUE(result.stdout != NULL);
    EXPECT_TRUE(strstr(result.stdout, "world") != NULL);
    
    process_free_result(&result);
}

void test_process_exec_argv() {
    TEST(Process_ExecArgv);
    char* argv[] = {"echo", "test_argv", NULL};
    process_config_t config = process_default_config();
    process_result_t result;
    process_error_t error;
    
    bool success = process_exec_argv(argv, &config, &result, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(result.exit_code, 0);
    
    process_free_result(&result);
}

void test_process_command_exists() {
    TEST(Process_CommandExists);
    bool exists = process_command_exists("echo");
    EXPECT_TRUE(exists);
    
    exists = process_command_exists("nonexistent_command_12345");
    EXPECT_FALSE(exists);
}

void test_process_which() {
    TEST(Process_Which);
    process_error_t error;
    char* path = process_which("ls", &error);
    
    EXPECT_TRUE(path != NULL);
    EXPECT_TRUE(strlen(path) > 0);
    
    free(path);
}

void test_process_get_pid() {
    TEST(Process_GetPid);
    pid_t pid = process_get_pid();
    EXPECT_TRUE(pid > 0);
}

void test_process_get_ppid() {
    TEST(Process_GetPpid);
    pid_t ppid = process_get_ppid();
    EXPECT_TRUE(ppid > 0);
}

void test_process_get_cwd() {
    TEST(Process_GetCwd);
    process_error_t error;
    char* cwd = process_get_cwd(&error);
    
    EXPECT_TRUE(cwd != NULL);
    EXPECT_TRUE(strlen(cwd) > 0);
    
    free(cwd);
}

void test_process_error_string() {
    TEST(Process_ErrorString);
    const char* msg = process_error_string(PROCESS_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = process_error_string(PROCESS_ERROR_FORK_FAILED);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_process_default_config();
    test_process_run();
    test_process_exec();
    test_process_exec_ex();
    test_process_exec_argv();
    test_process_command_exists();
    test_process_which();
    test_process_get_pid();
    test_process_get_ppid();
    test_process_get_cwd();
    test_process_error_string();

    return 0;
}
