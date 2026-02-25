/**
 * 进程管理演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/process.h"

static void demo_basic_exec(void) {
    printf("\n=== 演示 1: 基本命令执行 ===\n");

    int exit_code;
    char *output = process_exec("echo hello world", &exit_code);
    if (output) {
        printf("命令: echo hello world\n");
        printf("输出: %s", output);
        printf("退出码: %d\n", exit_code);
        free(output);
    } else {
        printf("执行失败\n");
    }
}

static void demo_run_function(void) {
    printf("\n=== 演示 2: process_run 函数 ===\n");

    int code = process_run("echo 'run function test'");
    printf("process_run 返回码: %d\n", code);

    code = process_run("ls /nonexistent 2>/dev/null");
    printf("无效命令返回码: %d\n", code);
}

static void demo_with_config(void) {
    printf("\n=== 演示 3: 带配置的执行 ===\n");

    process_config_t config = {
        .capture_stdout = true,
        .capture_stderr = true,
        .merge_stderr = false,
        .timeout_ms = 5000,
        .max_output_size = 4096,
        .working_dir = NULL
    };

    process_result_t result;
    process_error_t error;

    bool success = process_exec_ex("ls -la /tmp", &config, &result, &error);
    if (success) {
        printf("执行成功\n");
        printf("退出码: %d\n", result.exit_code);
        if (result.stdout) {
            printf("输出: %.50s...\n", result.stdout);
        }
        if (result.stderr) {
            printf("错误: %s\n", result.stderr);
        }
    } else {
        printf("执行失败: %d\n", error);
    }
}

static void demo_command_exists(void) {
    printf("\n=== 演示 4: 命令检查 ===\n");

    const char *commands[] = {"ls", "echo", "nonexistent_command_xyz", "gcc"};
    for (int i = 0; i < 4; i++) {
        bool exists = process_command_exists(commands[i]);
        printf("  %s: %s\n", commands[i], exists ? "存在" : "不存在");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    进程管理演示\n");
    printf("========================================\n");

    demo_basic_exec();
    demo_run_function();
    demo_with_config();
    demo_command_exists();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
