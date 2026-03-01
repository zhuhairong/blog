/**
 * 终端操作演示程序
 *
 * 功能：
 * - 颜色输出
 * - 进度条显示
 * - 终端控制
 * - 菜单显示
 * - 表格显示
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/terminal.h"

// 演示 1: 基本颜色输出
static void demo_color_output(void) {
    printf("\n=== 演示 1: 基本颜色输出 ===\n");

    // 使用预定义的ANSI颜色宏
    printf(TERM_ANSI_RED "红色文本" TERM_ANSI_RESET "\n");
    printf(TERM_ANSI_GREEN "绿色文本" TERM_ANSI_RESET "\n");
    printf(TERM_ANSI_YELLOW "黄色文本" TERM_ANSI_RESET "\n");
    printf(TERM_ANSI_BLUE "蓝色文本" TERM_ANSI_RESET "\n");
    printf(TERM_ANSI_MAGENTA "洋红色文本" TERM_ANSI_RESET "\n");
    printf(TERM_ANSI_CYAN "青色文本" TERM_ANSI_RESET "\n");

    // 使用term_printf函数
    term_printf(TERM_ANSI_RED, "使用term_printf: %s\n", "红色文本");
    term_printf(TERM_ANSI_GREEN, "使用term_printf: %s\n", "绿色文本");

    // 使用增强版term_printf_ex
    term_state_t state;
    term_state_init(&state);

    term_printf_ex(TERM_COLOR_RED, TERM_STYLE_BOLD_STYLE, "增强版: %s\n", "红色粗体文本");
    term_printf_ex(TERM_COLOR_BLUE, TERM_STYLE_UNDERLINE_STYLE, "增强版: %s\n", "蓝色下划线文本");
}

// 演示 2: 进度条显示
static void demo_progress_bar(void) {
    printf("\n=== 演示 2: 进度条显示 ===\n");

    term_progress_t progress;
    term_progress_init(&progress, 50, '#', '-');

    for (int i = 0; i <= 100; i += 5) {
        double progress_value = i / 100.0;
        term_progress_draw(&progress, progress_value, "正在处理...");
        usleep(100000); // 100ms
    }
    printf("\n");
}

// 演示 3: 终端控制
static void demo_terminal_control(void) {
    printf("\n=== 演示 3: 终端控制 ===\n");

    term_state_t state;
    term_state_init(&state);

    // 获取终端大小
    size_t width, height;
    term_error_t error = term_get_size(&width, &height, &state);
    if (error == TERM_OK) {
        printf("终端大小: %zu x %zu\n", width, height);
    } else {
        printf("获取终端大小失败: %s\n", term_strerror(&state));
    }

    // 清除屏幕
    printf("按任意键清除屏幕...");
    getchar();
    term_clear_screen(&state);
    printf("屏幕已清除\n");
}

// 演示 4: 菜单显示
static void menu_callback(void *data) {
    int choice = *((int *)data);
    printf("选择了菜单项 %d\n", choice);
}

static void demo_menu(void) {
    printf("\n=== 演示 4: 菜单显示 ===\n");

    term_state_t state;
    term_state_init(&state);

    term_config_t config;
    term_config_init(&config);

    int choice1 = 1, choice2 = 2, choice3 = 3;

    term_menu_item_t items[] = {
        {"选项 1", menu_callback, &choice1, true},
        {"选项 2", menu_callback, &choice2, true},
        {"选项 3", menu_callback, &choice3, true}
    };

    int selected = term_show_menu("测试菜单", items, 3, &config, &state);
    if (selected >= 0) {
        printf("用户选择了: %s\n", items[selected].text);
    }
}

// 演示 5: 表格显示
static void demo_table(void) {
    printf("\n=== 演示 5: 表格显示 ===\n");

    term_state_t state;
    term_state_init(&state);

    term_config_t config;
    term_config_init(&config);

    // 创建表格数据
    term_table_cell_t cells[2][3] = {
        {
            {"姓名", TERM_COLOR_BLUE, TERM_STYLE_BOLD_STYLE, 10, 1},
            {"年龄", TERM_COLOR_BLUE, TERM_STYLE_BOLD_STYLE, 10, 1},
            {"职业", TERM_COLOR_BLUE, TERM_STYLE_BOLD_STYLE, 15, 1}
        },
        {
            {"张三", TERM_COLOR_GREEN, TERM_STYLE_NORMAL, 10, 0},
            {"25", TERM_COLOR_GREEN, TERM_STYLE_NORMAL, 10, 1},
            {"工程师", TERM_COLOR_GREEN, TERM_STYLE_NORMAL, 15, 0}
        }
    };

    // 转换为二维指针
    term_table_cell_t *table[2] = {
        cells[0],
        cells[1]
    };

    term_show_table(table, 2, 3, &config, &state);
}

// 演示 6: 动画显示
static void demo_animation(void) {
    printf("\n=== 演示 6: 动画显示 ===\n");

    term_state_t state;
    term_state_init(&state);

    term_config_t config;
    term_config_init(&config);

    const char *frames[] = {
        "[     ]",
        "[=    ]",
        "[==   ]",
        "[===  ]",
        "[==== ]",
        "[=====]",
        "[ ====]",
        "[  ===]",
        "[   ==]",
        "[    =]"
    };

    term_show_animation(frames, 10, 100, &config, &state);
    printf("\n");
}

// 演示 7: 终端功能检查
static void demo_terminal_check(void) {
    printf("\n=== 演示 7: 终端功能检查 ===\n");

    term_state_t state;
    term_state_init(&state);

    bool colors_supported, ansi_supported;

    term_check_colors_supported(&colors_supported, &state);
    term_check_ansi_supported(&ansi_supported, &state);

    printf("颜色支持: %s\n", colors_supported ? "是" : "否");
    printf("ANSI支持: %s\n", ansi_supported ? "是" : "否");
}

// 演示 8: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 8: 错误处理 ===\n");

    term_state_t state;
    term_state_init(&state);

    // 测试错误情况
    term_error_t error = term_set_cursor_position(-1, -1, &state);
    if (error != TERM_OK) {
        printf("错误测试: %s\n", term_strerror(&state));
    }

    // 测试文件操作错误
    error = term_show_file("/nonexistent/file.txt", NULL, &state);
    if (error != TERM_OK) {
        printf("文件操作错误: %s\n", term_strerror(&state));
    }
}

int main(void) {
    printf("========================================\n");
    printf("    终端操作演示\n");
    printf("========================================\n");

    demo_color_output();
    demo_progress_bar();
    demo_terminal_control();
    demo_menu();
    demo_table();
    demo_animation();
    demo_terminal_check();
    demo_error_handling();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
