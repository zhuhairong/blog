#ifndef C_UTILS_BACKTRACE_H
#define C_UTILS_BACKTRACE_H

#include <stddef.h>
#include <stdio.h>

// 栈帧信息
typedef struct {
    void *address;
    char *symbol;
    char *filename;
    int line;
} backtrace_frame_t;

// 获取调用栈
// 返回获取的帧数，出错返回 0
size_t backtrace_get(void **buffer, size_t size);

// 获取调用栈帧的符号信息
// 返回符号数组（需要调用者用 free 释放）
char** backtrace_get_symbols(void *const *buffer, size_t size);

// 打印调用栈到标准输出
void backtrace_print(void);

// 打印调用栈到指定文件
void backtrace_print_to_file(FILE *fp);

// 获取详细的栈帧信息（如果可用）
// 返回实际获取的帧数
size_t backtrace_get_frames(backtrace_frame_t *frames, size_t max_frames);

// 释放栈帧信息
void backtrace_free_frames(backtrace_frame_t *frames, size_t count);

// 捕获当前异常/信号上下文中的调用栈（需要信号处理中使用）
// context 为 ucontext_t* 类型（平台相关）
size_t backtrace_from_context(void *context, void **buffer, size_t size);

#endif // C_UTILS_BACKTRACE_H
