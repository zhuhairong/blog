#include "terminal.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

// 获取ANSI颜色码 - 使用if-else避免宏和枚举冲突
static const char* term_color_to_ansi(term_color_t color) {
    if (color == TERM_COLOR_BLACK) return "\x1b[30m";
    if (color == TERM_COLOR_RED) return "\x1b[31m";
    if (color == TERM_COLOR_GREEN) return "\x1b[32m";
    if (color == TERM_COLOR_YELLOW) return "\x1b[33m";
    if (color == TERM_COLOR_BLUE) return "\x1b[34m";
    if (color == TERM_COLOR_MAGENTA) return "\x1b[35m";
    if (color == TERM_COLOR_CYAN) return "\x1b[36m";
    if (color == TERM_COLOR_WHITE) return "\x1b[37m";
    if (color == TERM_COLOR_BRIGHT_BLACK) return "\x1b[90m";
    if (color == TERM_COLOR_BRIGHT_RED) return "\x1b[91m";
    if (color == TERM_COLOR_BRIGHT_GREEN) return "\x1b[92m";
    if (color == TERM_COLOR_BRIGHT_YELLOW) return "\x1b[93m";
    if (color == TERM_COLOR_BRIGHT_BLUE) return "\x1b[94m";
    if (color == TERM_COLOR_BRIGHT_MAGENTA) return "\x1b[95m";
    if (color == TERM_COLOR_BRIGHT_CYAN) return "\x1b[96m";
    if (color == TERM_COLOR_BRIGHT_WHITE) return "\x1b[97m";
    return "\x1b[0m";
}

// 获取ANSI样式码
static const char* term_style_to_ansi(term_style_t style) {
    if (style == TERM_STYLE_BOLD_STYLE) return "\x1b[1m";
    if (style == TERM_STYLE_UNDERLINE_STYLE) return "\x1b[4m";
    if (style == TERM_STYLE_ITALIC_STYLE) return "\x1b[3m";
    if (style == TERM_STYLE_BLINK) return "\x1b[5m";
    if (style == TERM_STYLE_REVERSE) return "\x1b[7m";
    if (style == TERM_STYLE_HIDDEN) return "\x1b[8m";
    return "";
}

// 打印带颜色的文本
void term_printf(const char *color, const char *fmt, ...) {
    printf("%s", color);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("%s", TERM_ANSI_RESET);
}

// 增强版带颜色的文本打印
term_error_t term_vprintf_ex(term_color_t color, term_style_t style,
                           const char *fmt, va_list args,
                           const term_config_t *config,
                           term_state_t *state) {
    if (!fmt) return TERM_ERROR_INVALID_PARAMS;
    
    (void)config;
    
    // 应用颜色
    if (color != TERM_COLOR_NONE) {
        printf("%s", term_color_to_ansi(color));
    }
    
    // 应用样式
    if (style != TERM_STYLE_NORMAL) {
        printf("%s", term_style_to_ansi(style));
    }
    
    // 打印文本
    int result = vprintf(fmt, args);
    
    // 重置
    if (color != TERM_COLOR_NONE || style != TERM_STYLE_NORMAL) {
        printf("\x1b[0m");
    }
    
    if (result < 0) {
        return TERM_ERROR_PRINTF;
    }
    
    if (state) {
        state->prints++;
        state->last_error = TERM_OK;
    }
    
    return TERM_OK;
}

// 增强版带颜色的文本打印
term_error_t term_printf_ex(term_color_t color, term_style_t style,
                          const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    term_error_t result = term_vprintf_ex(color, style, fmt, args, NULL, NULL);
    va_end(args);
    return result;
}

// 显示进度条
void term_progress_draw(term_progress_t *tp, double progress, const char *label) {
    if (!tp) return;
    
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
    
    int filled = (int)(tp->width * progress);
    printf("\r%s [", label ? label : "");
    for (int i = 0; i < tp->width; i++) {
        putchar(i < filled ? tp->fill : tp->empty);
    }
    printf("] %d%%", (int)(progress * 100));
    if (progress == 1.0) printf("\n");
    fflush(stdout);
}

// 增强版进度条显示
term_error_t term_progress_draw_ex(term_progress_t *tp, double progress,
                                 const char *label,
                                 const term_config_t *config,
                                 term_state_t *state) {
    if (!tp) return TERM_ERROR_INVALID_PARAMS;
    
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
    
    (void)config;
    
    int filled = (int)(tp->width * progress);
    printf("\r%s [", label ? label : "");
    for (int i = 0; i < tp->width; i++) {
        putchar(i < filled ? tp->fill : tp->empty);
    }
    printf("] %d%%", (int)(progress * 100));
    if (progress == 1.0) printf("\n");
    fflush(stdout);
    
    if (state) {
        state->progress_updates++;
        state->last_error = TERM_OK;
    }
    
    return TERM_OK;
}

// 初始化进度条
term_error_t term_progress_init(term_progress_t *tp, int width,
                              char fill, char empty) {
    if (!tp) return TERM_ERROR_INVALID_PARAMS;
    
    tp->width = width > 0 ? width : 50;
    tp->fill = fill ? fill : '#';
    tp->empty = empty ? empty : '-';
    
    return TERM_OK;
}

// 获取终端大小
term_error_t term_get_size(size_t *width, size_t *height,
                         term_state_t *state) {
    if (!width || !height) return TERM_ERROR_INVALID_PARAMS;
    
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        *width = 80;
        *height = 24;
        if (state) state->last_error = TERM_ERROR_TERMINAL_SIZE;
        return TERM_ERROR_TERMINAL_SIZE;
    }
    
    *width = ws.ws_col;
    *height = ws.ws_row;
    
    if (state) {
        state->terminal_width = *width;
        state->terminal_height = *height;
        state->last_error = TERM_OK;
    }
    
    return TERM_OK;
}

// 设置光标位置
term_error_t term_set_cursor_position(int x, int y,
                                    term_state_t *state) {
    (void)state;
    printf("\x1b[%d;%dH", y, x);
    fflush(stdout);
    return TERM_OK;
}

// 隐藏光标
term_error_t term_hide_cursor(term_state_t *state) {
    (void)state;
    printf("\x1b[?25l");
    fflush(stdout);
    return TERM_OK;
}

// 显示光标
term_error_t term_show_cursor(term_state_t *state) {
    (void)state;
    printf("\x1b[?25h");
    fflush(stdout);
    return TERM_OK;
}

// 清除屏幕
term_error_t term_clear_screen(term_state_t *state) {
    (void)state;
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
    return TERM_OK;
}

// 清除行
term_error_t term_clear_line(term_state_t *state) {
    (void)state;
    printf("\r\x1b[K");
    fflush(stdout);
    return TERM_OK;
}

// 读取键盘输入
term_error_t term_read_keyboard(char *buffer, size_t size,
                              term_state_t *state) {
    if (!buffer || size == 0) return TERM_ERROR_INVALID_PARAMS;
    
    (void)state;
    
    ssize_t n = read(STDIN_FILENO, buffer, size - 1);
    if (n < 0) {
        return TERM_ERROR_KEYBOARD;
    }
    
    buffer[n] = '\0';
    return TERM_OK;
}

// 显示菜单
int term_show_menu(const char *title, term_menu_item_t *items,
                  size_t count, const term_config_t *config,
                  term_state_t *state) {
    (void)config;
    (void)state;
    
    if (title) {
        printf("\n%s\n", title);
        for (size_t i = 0; i < strlen(title); i++) printf("-");
        printf("\n");
    }
    
    for (size_t i = 0; i < count; i++) {
        printf("%zu. %s\n", i + 1, items[i].text);
    }
    
    printf("\n请选择: ");
    fflush(stdout);
    
    int choice = 0;
    if (scanf("%d", &choice) != 1) {
        // 清除输入缓冲区
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }
    
    return (choice > 0 && (size_t)choice <= count) ? choice - 1 : -1;
}

// 显示表格
term_error_t term_show_table(term_table_cell_t **cells,
                           size_t rows, size_t cols,
                           const term_config_t *config,
                           term_state_t *state) {
    (void)config;
    (void)state;
    
    if (!cells) return TERM_ERROR_INVALID_PARAMS;
    
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (cells[i][j].text) {
                printf("%s", cells[i][j].text);
            }
            if (j < cols - 1) printf(" | ");
        }
        printf("\n");
    }
    
    return TERM_OK;
}

// 显示动画
term_error_t term_show_animation(const char **frames, size_t count,
                                uint64_t delay_ms,
                                const term_config_t *config,
                                term_state_t *state) {
    (void)config;
    (void)state;
    
    if (!frames) return TERM_ERROR_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        printf("\r%s", frames[i]);
        fflush(stdout);
        usleep((useconds_t)(delay_ms * 1000));
    }
    printf("\n");
    
    return TERM_OK;
}

// 从文件显示内容
term_error_t term_show_file(const char *filename,
                          const term_config_t *config,
                          term_state_t *state) {
    (void)config;
    (void)state;
    
    if (!filename) return TERM_ERROR_INVALID_PARAMS;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) return TERM_ERROR_FILE_OPEN;
    
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    
    fclose(fp);
    return TERM_OK;
}

// 保存终端输出到文件
term_error_t term_save_to_file(const char *filename, const char *text,
                             term_state_t *state) {
    (void)state;
    
    if (!filename || !text) return TERM_ERROR_INVALID_PARAMS;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) return TERM_ERROR_FILE_OPEN;
    
    fprintf(fp, "%s", text);
    fclose(fp);
    
    return TERM_OK;
}

// 检查终端是否支持颜色
term_error_t term_check_colors_supported(bool *supported,
                                        term_state_t *state) {
    if (!supported) return TERM_ERROR_INVALID_PARAMS;
    
    *supported = isatty(STDOUT_FILENO);
    
    if (state) {
        state->colors_supported = *supported;
    }
    
    return TERM_OK;
}

// 检查终端是否支持ANSI
term_error_t term_check_ansi_supported(bool *supported,
                                      term_state_t *state) {
    if (!supported) return TERM_ERROR_INVALID_PARAMS;
    
    *supported = isatty(STDOUT_FILENO);
    
    if (state) {
        state->ansi_supported = *supported;
    }
    
    return TERM_OK;
}

// 设置终端模式
term_error_t term_set_mode(bool raw_mode,
                         term_state_t *state) {
    (void)raw_mode;
    (void)state;
    return TERM_OK;
}

// 重置终端模式
term_error_t term_reset_mode(term_state_t *state) {
    (void)state;
    return TERM_OK;
}

// 获取终端统计信息
term_error_t term_get_statistics(term_state_t *state) {
    if (!state) return TERM_ERROR_INVALID_PARAMS;
    
    printf("Terminal Statistics:\n");
    printf("  Prints: %zu\n", state->prints);
    printf("  Progress updates: %zu\n", state->progress_updates);
    printf("  Terminal size: %zux%zu\n", state->terminal_width, state->terminal_height);
    printf("  Colors supported: %s\n", state->colors_supported ? "yes" : "no");
    printf("  ANSI supported: %s\n", state->ansi_supported ? "yes" : "no");
    
    return TERM_OK;
}

// 重置终端状态
void term_reset_state(term_state_t *state) {
    if (!state) return;
    
    memset(state, 0, sizeof(term_state_t));
    state->last_error = TERM_OK;
}

// 获取最后一次错误信息
const char* term_strerror(const term_state_t *state) {
    if (!state) return "Invalid state";
    
    term_error_t err = state->last_error;
    if (err == TERM_OK) return "Success";
    if (err == TERM_ERROR_INVALID_PARAMS) return "Invalid parameters";
    if (err == TERM_ERROR_PRINTF) return "Printf error";
    if (err == TERM_ERROR_PROGRESS) return "Progress error";
    if (err == TERM_ERROR_TERMINAL_SIZE) return "Terminal size error";
    if (err == TERM_ERROR_CURSOR) return "Cursor error";
    if (err == TERM_ERROR_KEYBOARD) return "Keyboard error";
    if (err == TERM_ERROR_MEMORY) return "Memory error";
    if (err == TERM_ERROR_FILE_OPEN) return "File open error";
    if (err == TERM_ERROR_FILE_READ) return "File read error";
    if (err == TERM_ERROR_FILE_WRITE) return "File write error";
    if (err == TERM_ERROR_COLOR_UNSUPPORTED) return "Color unsupported";
    if (err == TERM_ERROR_STYLE_UNSUPPORTED) return "Style unsupported";
    if (err == TERM_ERROR_MODE) return "Mode error";
    if (err == TERM_ERROR_ANSI_UNSUPPORTED) return "ANSI unsupported";
    return "Unknown error";
}

// 初始化配置为默认值
void term_config_init(term_config_t *config) {
    if (!config) return;
    
    config->enable_colors = true;
    config->enable_styles = true;
    config->enable_ansi = true;
    config->enable_statistics = false;
    config->use_unicode = false;
    config->enable_cursor = true;
    config->enable_keyboard = true;
    config->max_width = 0;
    config->max_height = 0;
    config->default_color = TERM_COLOR_NONE;
}

// 初始化状态为默认值
void term_state_init(term_state_t *state) {
    if (!state) return;
    
    memset(state, 0, sizeof(term_state_t));
    state->last_error = TERM_OK;
    state->terminal_width = 80;
    state->terminal_height = 24;
}
