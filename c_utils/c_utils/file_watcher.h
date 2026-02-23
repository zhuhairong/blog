#ifndef C_UTILS_FILE_WATCHER_H
#define C_UTILS_FILE_WATCHER_H

#include <stdbool.h>
#include <time.h>
#include <stddef.h>

// 文件监控事件类型
typedef enum {
    FILE_WATCHER_EVENT_CREATE,
    FILE_WATCHER_EVENT_MODIFY,
    FILE_WATCHER_EVENT_DELETE,
    FILE_WATCHER_EVENT_RENAME,
    FILE_WATCHER_EVENT_ERROR
} file_watcher_event_t;

// 文件监控错误码
typedef enum {
    FILE_WATCHER_OK = 0,
    FILE_WATCHER_ERROR_INVALID_PARAM,
    FILE_WATCHER_ERROR_FILE_NOT_FOUND,
    FILE_WATCHER_ERROR_MEMORY_ALLOC,
    FILE_WATCHER_ERROR_DIR_OPEN,
    FILE_WATCHER_ERROR_DIR_READ,
    FILE_WATCHER_ERROR_PLATFORM_UNSUPPORTED
} file_watcher_error_t;

// 文件监控事件回调
typedef void (*file_watcher_callback_t)(const char *path, file_watcher_event_t event, void *user_data);

// 文件监控项
typedef struct {
    char *path;
    time_t last_mtime;
    bool is_directory;
    struct file_watcher_item_s *next;
} file_watcher_item_s;

// 文件监控器
typedef struct {
    file_watcher_item_s *items;
    size_t count;
    size_t capacity;
    bool recursive;
    file_watcher_callback_t callback;
    void *user_data;
    bool has_error;
    file_watcher_error_t error;
    char error_msg[256];
} file_watcher_t;

// 文件监控选项
typedef struct {
    bool recursive;
    bool follow_symlinks;
    bool watch_hidden;
    bool use_polling;
    int poll_interval_ms;
} file_watcher_options_t;

// 默认文件监控选项
// 返回: 默认选项
file_watcher_options_t file_watcher_default_options(void);

// 创建文件监控器
// options: 选项（可为 NULL 使用默认选项）
// callback: 事件回调（可为 NULL）
// user_data: 用户数据（可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回监控器指针，失败返回 NULL
file_watcher_t* file_watcher_create(const file_watcher_options_t *options, file_watcher_callback_t callback, void *user_data, file_watcher_error_t *error);

// 释放文件监控器
// fw: 文件监控器
void file_watcher_free(file_watcher_t *fw);

// 添加监控文件或目录
// fw: 文件监控器
// path: 文件或目录路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool file_watcher_add(file_watcher_t *fw, const char *path, file_watcher_error_t *error);

// 移除监控文件或目录
// fw: 文件监控器
// path: 文件或目录路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool file_watcher_remove(file_watcher_t *fw, const char *path, file_watcher_error_t *error);

// 检查文件变化
// fw: 文件监控器
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool file_watcher_check(file_watcher_t *fw, file_watcher_error_t *error);

// 批量添加监控
// fw: 文件监控器
// paths: 路径数组
// count: 路径数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool file_watcher_add_batch(file_watcher_t *fw, const char **paths, size_t count, file_watcher_error_t *error);

// 获取监控项数量
// fw: 文件监控器
// 返回: 监控项数量
size_t file_watcher_get_count(const file_watcher_t *fw);

// 检查文件监控器是否有错误
// fw: 文件监控器
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool file_watcher_has_error(const file_watcher_t *fw, file_watcher_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* file_watcher_strerror(file_watcher_error_t error);

#endif // C_UTILS_FILE_WATCHER_H
