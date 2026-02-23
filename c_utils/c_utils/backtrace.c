#include "backtrace.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif

// 获取调用栈
size_t backtrace_get(void **buffer, size_t size) {
    if (!buffer || size == 0) return 0;
    return backtrace(buffer, (int)size);
}

// 获取符号信息（重命名以避免与系统函数冲突）
char** backtrace_get_symbols(void *const *buffer, size_t size) {
    if (!buffer || size == 0) return NULL;
    return backtrace_symbols((void* const*)buffer, (int)size);
}

// 打印调用栈到标准输出
void backtrace_print(void) {
    void *array[64];
    size_t size = backtrace(array, 64);
    char **strings = backtrace_symbols(array, size);
    
    if (strings) {
        printf("Backtrace (%zd frames):\n", size);
        for (size_t i = 0; i < size; i++) {
            printf("  #%zu %s\n", i, strings[i]);
        }
        free(strings);
    } else {
        printf("Failed to get backtrace symbols\n");
    }
}

// 打印调用栈到指定文件
void backtrace_print_to_file(FILE *fp) {
    if (!fp) return;
    
    void *array[64];
    size_t size = backtrace(array, 64);
    char **strings = backtrace_symbols(array, size);
    
    if (strings) {
        fprintf(fp, "Backtrace (%zd frames):\n", size);
        for (size_t i = 0; i < size; i++) {
            fprintf(fp, "  #%zu %s\n", i, strings[i]);
        }
        free(strings);
    } else {
        fprintf(fp, "Failed to get backtrace symbols\n");
    }
}

// 解析符号信息（简化版）
static void parse_symbol(const char *symbol, backtrace_frame_t *frame) {
    if (!symbol || !frame) return;
    
    // 复制原始符号
    frame->symbol = strdup(symbol);
    
    // 尝试解析地址
    // 格式通常为: "./program(function+offset) [address]"
    const char *addr_start = strchr(symbol, '[');
    if (addr_start) {
        sscanf(addr_start, "[%p]", &frame->address);
    }
    
    // 尝试解析函数名
    const char *func_start = strchr(symbol, '(');
    const char *func_end = strchr(symbol, '+');
    if (func_start && func_end && func_end > func_start) {
        size_t len = func_end - func_start - 1;
        if (len > 0 && len < 256) {
            char *func_name = malloc(len + 1);
            if (func_name) {
                strncpy(func_name, func_start + 1, len);
                func_name[len] = '\0';
                
                // 尝试 demangle C++ 符号（简化处理）
                if (func_name[0] == '_' && func_name[1] == 'Z') {
                    // 这是 mangled C++ 符号，保留原样
                }
                
                free(frame->symbol);
                frame->symbol = func_name;
            }
        }
    }
}

// 获取详细的栈帧信息
size_t backtrace_get_frames(backtrace_frame_t *frames, size_t max_frames) {
    if (!frames || max_frames == 0) return 0;
    
    void *buffer[64];
    size_t size = backtrace(buffer, max_frames < 64 ? (int)max_frames : 64);
    char **strings = backtrace_symbols(buffer, size);
    
    if (!strings) return 0;
    
    memset(frames, 0, sizeof(backtrace_frame_t) * size);
    
    for (size_t i = 0; i < size; i++) {
        frames[i].address = buffer[i];
        parse_symbol(strings[i], &frames[i]);
    }
    
    free(strings);
    return size;
}

// 释放栈帧信息
void backtrace_free_frames(backtrace_frame_t *frames, size_t count) {
    if (!frames) return;
    
    for (size_t i = 0; i < count; i++) {
        free(frames[i].symbol);
        free(frames[i].filename);
    }
}

// 从信号上下文获取调用栈（简化实现）
size_t backtrace_from_context(void *context, void **buffer, size_t size) {
    (void)context;
    if (!buffer || size == 0) return 0;
    // 简化实现：回退到普通 backtrace
    return backtrace(buffer, (int)size);
}
