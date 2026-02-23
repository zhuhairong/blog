#ifndef C_UTILS_TERMINAL_H
#define C_UTILS_TERMINAL_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

// ANSI颜色字符串定义
#define TERM_ANSI_RED     "\x1b[31m"
#define TERM_ANSI_GREEN   "\x1b[32m"
#define TERM_ANSI_YELLOW  "\x1b[33m"
#define TERM_ANSI_BLUE    "\x1b[34m"
#define TERM_ANSI_MAGENTA "\x1b[35m"
#define TERM_ANSI_CYAN    "\x1b[36m"
#define TERM_ANSI_RESET   "\x1b[0m"

// 文本样式定义
#define TERM_STYLE_BOLD_STR      "\x1b[1m"
#define TERM_STYLE_UNDERLINE_STR "\x1b[4m"
#define TERM_STYLE_ITALIC_STR    "\x1b[3m"
#define TERM_STYLE_RESET_STR     "\x1b[0m"

typedef enum {
    TERM_OK = 0,
    TERM_ERROR_INVALID_PARAMS = -1,
    TERM_ERROR_PRINTF = -2,
    TERM_ERROR_PROGRESS = -3,
    TERM_ERROR_TERMINAL_SIZE = -4,
    TERM_ERROR_CURSOR = -5,
    TERM_ERROR_KEYBOARD = -6,
    TERM_ERROR_MEMORY = -7,
    TERM_ERROR_FILE_OPEN = -8,
    TERM_ERROR_FILE_READ = -9,
    TERM_ERROR_FILE_WRITE = -10,
    TERM_ERROR_COLOR_UNSUPPORTED = -11,
    TERM_ERROR_STYLE_UNSUPPORTED = -12,
    TERM_ERROR_MODE = -13,
    TERM_ERROR_ANSI_UNSUPPORTED = -14
} term_error_t;

typedef enum {
    TERM_COLOR_NONE = 0,
    TERM_COLOR_BLACK,
    TERM_COLOR_RED,
    TERM_COLOR_GREEN,
    TERM_COLOR_YELLOW,
    TERM_COLOR_BLUE,
    TERM_COLOR_MAGENTA,
    TERM_COLOR_CYAN,
    TERM_COLOR_WHITE,
    TERM_COLOR_BRIGHT_BLACK,
    TERM_COLOR_BRIGHT_RED,
    TERM_COLOR_BRIGHT_GREEN,
    TERM_COLOR_BRIGHT_YELLOW,
    TERM_COLOR_BRIGHT_BLUE,
    TERM_COLOR_BRIGHT_MAGENTA,
    TERM_COLOR_BRIGHT_CYAN,
    TERM_COLOR_BRIGHT_WHITE
} term_color_t;

typedef enum {
    TERM_STYLE_NORMAL = 0,
    TERM_STYLE_BOLD_STYLE,
    TERM_STYLE_UNDERLINE_STYLE,
    TERM_STYLE_ITALIC_STYLE,
    TERM_STYLE_BLINK,
    TERM_STYLE_REVERSE,
    TERM_STYLE_HIDDEN
} term_style_t;

typedef struct {
    bool enable_colors;            // 启用颜色
    bool enable_styles;            // 启用样式
    bool enable_ansi;              // 启用ANSI转义序列
    bool enable_statistics;         // 启用统计
    bool use_unicode;              // 使用Unicode
    bool enable_cursor;            // 启用光标控制
    bool enable_keyboard;          // 启用键盘输入
    size_t max_width;              // 最大宽度
    size_t max_height;             // 最大高度
    term_color_t default_color;     // 默认颜色
} term_config_t;

typedef struct {
    term_error_t last_error;        // 最后一次错误
    size_t prints;                  // 打印次数
    size_t progress_updates;        // 进度条更新次数
    bool is_initialized;            // 是否初始化
    size_t terminal_width;          // 终端宽度
    size_t terminal_height;         // 终端高度
    bool colors_supported;          // 是否支持颜色
    bool ansi_supported;            // 是否支持ANSI
} term_state_t;

// 进度条结构
typedef struct {
    int width;
    char fill;
    char empty;
} term_progress_t;

// 终端菜单项
typedef struct {
    char *text;
    void (*callback)(void *);
    void *data;
    bool enabled;
} term_menu_item_t;

// 终端表格单元格
typedef struct {
    char *text;
    term_color_t color;
    term_style_t style;
    int width;
    int alignment; // 0: left, 1: center, 2: right
} term_table_cell_t;

/**
 * @brief 打印带颜色的文本
 * @param color 颜色字符串
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
void term_printf(const char *color, const char *fmt, ...);

/**
 * @brief 增强版带颜色的文本打印
 * @param color 颜色枚举值
 * @param style 样式枚举值
 * @param fmt 格式字符串
 * @param args 可变参数列表
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_vprintf_ex(term_color_t color, term_style_t style,
                           const char *fmt, va_list args,
                           const term_config_t *config,
                           term_state_t *state);

/**
 * @brief 增强版带颜色的文本打印
 * @param color 颜色枚举值
 * @param style 样式枚举值
 * @param fmt 格式字符串
 * @param ... 可变参数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_printf_ex(term_color_t color, term_style_t style,
                          const char *fmt, ...);

/**
 * @brief 显示进度条 (progress: 0.0 ~ 1.0)
 * @param tp 进度条结构体
 * @param progress 进度值
 * @param label 标签
 */
void term_progress_draw(term_progress_t *tp, double progress, const char *label);

/**
 * @brief 增强版进度条显示
 * @param tp 进度条结构体
 * @param progress 进度值
 * @param label 标签
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_progress_draw_ex(term_progress_t *tp, double progress,
                                 const char *label,
                                 const term_config_t *config,
                                 term_state_t *state);

/**
 * @brief 初始化进度条
 * @param tp 进度条结构体
 * @param width 宽度
 * @param fill 填充字符
 * @param empty 空白字符
 * @return 错误码
 */
term_error_t term_progress_init(term_progress_t *tp, int width,
                              char fill, char empty);

/**
 * @brief 获取终端大小
 * @param width 输出宽度
 * @param height 输出高度
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_get_size(size_t *width, size_t *height,
                         term_state_t *state);

/**
 * @brief 设置光标位置
 * @param x X坐标
 * @param y Y坐标
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_set_cursor_position(int x, int y,
                                    term_state_t *state);

/**
 * @brief 隐藏光标
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_hide_cursor(term_state_t *state);

/**
 * @brief 显示光标
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_show_cursor(term_state_t *state);

/**
 * @brief 清除屏幕
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_clear_screen(term_state_t *state);

/**
 * @brief 清除行
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_clear_line(term_state_t *state);

/**
 * @brief 读取键盘输入
 * @param buffer 输入缓冲区
 * @param size 缓冲区大小
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_read_keyboard(char *buffer, size_t size,
                              term_state_t *state);

/**
 * @brief 显示菜单
 * @param title 菜单标题
 * @param items 菜单项数组
 * @param count 菜单项数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 选择的菜单项索引
 */
int term_show_menu(const char *title, term_menu_item_t *items,
                  size_t count, const term_config_t *config,
                  term_state_t *state);

/**
 * @brief 显示表格
 * @param cells 表格单元格数组
 * @param rows 行数
 * @param cols 列数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_show_table(term_table_cell_t **cells,
                           size_t rows, size_t cols,
                           const term_config_t *config,
                           term_state_t *state);

/**
 * @brief 显示动画
 * @param frames 动画帧数组
 * @param count 帧数量
 * @param delay_ms 帧延迟
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_show_animation(const char **frames, size_t count,
                                uint64_t delay_ms,
                                const term_config_t *config,
                                term_state_t *state);

/**
 * @brief 从文件显示内容
 * @param filename 文件名
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_show_file(const char *filename,
                          const term_config_t *config,
                          term_state_t *state);

/**
 * @brief 保存终端输出到文件
 * @param filename 文件名
 * @param text 要保存的文本
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_save_to_file(const char *filename, const char *text,
                             term_state_t *state);

/**
 * @brief 检查终端是否支持颜色
 * @param supported 输出支持状态
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_check_colors_supported(bool *supported,
                                        term_state_t *state);

/**
 * @brief 检查终端是否支持ANSI
 * @param supported 输出支持状态
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_check_ansi_supported(bool *supported,
                                      term_state_t *state);

/**
 * @brief 设置终端模式
 * @param raw_mode 是否使用原始模式
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_set_mode(bool raw_mode,
                         term_state_t *state);

/**
 * @brief 重置终端模式
 * @param state 状态输出
 * @return 错误码
 */
term_error_t term_reset_mode(term_state_t *state);

/**
 * @brief 获取终端统计信息
 * @param state 状态结构
 * @return 错误码
 */
term_error_t term_get_statistics(term_state_t *state);

/**
 * @brief 重置终端状态
 * @param state 状态结构
 */
void term_reset_state(term_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* term_strerror(const term_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void term_config_init(term_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void term_state_init(term_state_t *state);

#endif // C_UTILS_TERMINAL_H
