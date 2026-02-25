/**
 * @file demo_backtrace.c
 * @brief 调用栈回溯演示
 *
 * 本演示展示了调用栈回溯功能的各种用法，包括：
 * - 获取调用栈
 * - 打印调用栈
 * - 获取符号信息
 * - 详细栈帧信息
 * - 信号处理中的回溯
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "../c_utils/backtrace.h"

// 函数声明
void function_d(void);
void function_c(void);
void function_b(void);
void function_a(void);

// 深层调用链 - 第 4 层
void function_d(void) {
    printf("\n=== 在 function_d 中获取调用栈 ===\n");
    backtrace_print();
}

// 深层调用链 - 第 3 层
void function_c(void) {
    function_d();
}

// 深层调用链 - 第 2 层
void function_b(void) {
    function_c();
}

// 深层调用链 - 第 1 层
void function_a(void) {
    function_b();
}

// 演示 1: 基本调用栈获取
static void demo_basic_backtrace(void) {
    printf("\n=== 演示 1: 基本调用栈获取 ===\n");

    void* buffer[32];
    size_t size = backtrace_get(buffer, 32);

    printf("获取到 %zu 个栈帧:\n", size);
    for (size_t i = 0; i < size && i < 10; i++) {
        printf("  [%zu] %p\n", i, buffer[i]);
    }
    if (size > 10) {
        printf("  ... (还有 %zu 个帧)\n", size - 10);
    }
}

// 演示 2: 获取符号信息
static void demo_symbols(void) {
    printf("\n=== 演示 2: 获取符号信息 ===\n");

    void* buffer[32];
    size_t size = backtrace_get(buffer, 32);

    printf("获取到 %zu 个栈帧，符号信息:\n", size);
    char** symbols = backtrace_get_symbols(buffer, size);

    if (symbols) {
        for (size_t i = 0; i < size && i < 10; i++) {
            printf("  [%zu] %s\n", i, symbols[i]);
        }
        // 注意：实际需要释放 symbols 数组
        // free(symbols);
    } else {
        printf("  无法获取符号信息\n");
    }
}

// 演示 3: 打印到文件
static void demo_print_to_file(void) {
    printf("\n=== 演示 3: 打印调用栈到文件 ===\n");

    const char* filename = "/tmp/backtrace_demo.txt";
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建文件 %s\n", filename);
        return;
    }

    fprintf(fp, "=== 调用栈回溯 ===\n");
    backtrace_print_to_file(fp);
    fclose(fp);

    printf("调用栈已写入 %s\n", filename);

    // 读取并显示文件内容
    fp = fopen(filename, "r");
    if (fp) {
        printf("\n文件内容预览:\n");
        char line[256];
        int lines = 0;
        while (fgets(line, sizeof(line), fp) && lines < 10) {
            printf("  %s", line);
            lines++;
        }
        fclose(fp);
    }
}

// 演示 4: 深层调用链
static void demo_deep_call_stack(void) {
    printf("\n=== 演示 4: 深层调用链回溯 ===\n");
    printf("调用链: main -> function_a -> function_b -> function_c -> function_d\n");
    function_a();
}

// 演示 5: 详细栈帧信息
static void demo_detailed_frames(void) {
    printf("\n=== 演示 5: 详细栈帧信息 ===\n");

    backtrace_frame_t frames[32];
    size_t count = backtrace_get_frames(frames, 32);

    printf("获取到 %zu 个详细栈帧:\n", count);
    for (size_t i = 0; i < count && i < 5; i++) {
        printf("  [%zu]\n", i);
        printf("    地址: %p\n", frames[i].address);
        printf("    符号: %s\n",
               frames[i].symbol ? frames[i].symbol : "(未知)");
        printf("    文件: %s\n",
               frames[i].filename ? frames[i].filename : "(未知)");
        printf("    行号: %d\n", frames[i].line);
    }

    // 释放帧信息
    backtrace_free_frames(frames, count);
}

// 演示 6: 递归函数回溯
static void recursive_function(int depth) {
    if (depth <= 0) {
        printf("\n=== 递归深度为 0，获取调用栈 ===\n");
        backtrace_print();
        return;
    }

    printf("递归深度: %d\n", depth);
    recursive_function(depth - 1);
}

static void demo_recursive(void) {
    printf("\n=== 演示 6: 递归函数回溯 ===\n");
    recursive_function(5);
}

// 信号处理函数
static void signal_handler(int sig) {
    printf("\n=== 捕获到信号 %d (%s) ===\n", sig,
           sig == SIGSEGV ? "SIGSEGV" :
           sig == SIGABRT ? "SIGABRT" : "UNKNOWN");

    printf("调用栈回溯:\n");
    backtrace_print();

    // 恢复默认处理并重新触发信号
    signal(sig, SIG_DFL);
    raise(sig);
}

// 演示 7: 信号处理中的回溯（仅演示，不实际触发）
static void demo_signal_handler(void) {
    printf("\n=== 演示 7: 信号处理中的回溯 ===\n");
    printf("说明: 此演示展示如何在信号处理中使用回溯\n");
    printf("      实际信号处理需要设置信号处理器\n\n");

    printf("设置信号处理器...\n");
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);

    printf("信号处理器已设置\n");
    printf("在真实场景中，程序崩溃时会自动打印调用栈\n");

    // 恢复正常信号处理
    signal(SIGSEGV, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

// 演示 8: 错误处理场景
static void process_data(const char* data) {
    if (!data) {
        printf("错误: 接收到 NULL 指针\n");
        printf("当前调用栈:\n");
        backtrace_print();
        return;
    }
    printf("处理数据: %s\n", data);
}

static void validate_input(const char* input) {
    process_data(input);
}

static void demo_error_handling(void) {
    printf("\n=== 演示 8: 错误处理场景 ===\n");
    printf("模拟错误情况，打印调用栈以定位问题:\n");
    validate_input(NULL);
}

int main(void) {
    printf("========================================\n");
    printf("    调用栈回溯演示程序\n");
    printf("========================================\n");

    demo_basic_backtrace();
    demo_symbols();
    demo_print_to_file();
    demo_deep_call_stack();
    demo_detailed_frames();
    demo_recursive();
    demo_signal_handler();
    demo_error_handling();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
