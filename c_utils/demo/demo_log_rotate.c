/**
 * 日志滚动演示程序
 *
 * 功能：
 * - 按大小滚动日志
 * - 按时间滚动日志
 * - 备份管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/log_rotate.h"

// 演示 1: 基本滚动
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本日志滚动 ===\n");

    const char *logfile = "/tmp/demo_rotate.log";

    // 创建一个测试日志文件
    FILE *fp = fopen(logfile, "w");
    if (fp) {
        // 写入一些内容
        for (int i = 0; i < 100; i++) {
            fprintf(fp, "Log line %d: This is a test log entry for rotation demonstration.\n", i);
        }
        fclose(fp);

        printf("创建日志文件: %s\n", logfile);

        size_t size;
        log_rotate_error_t error = log_rotate_get_file_size(logfile, &size);
        if (error == LOG_ROTATE_OK) {
            printf("文件大小: %zu 字节\n", size);
        }

        printf("\n执行日志滚动 (max_size=500, max_backups=3)...\n");
        log_rotate(logfile, 500, 3);

        printf("滚动完成\n");
    }
}

// 演示 2: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 2: 日志滚动配置 ===\n");

    log_rotate_config_t config;
    log_rotate_get_default_config(&config);

    printf("默认配置:\n");
    printf("  最大文件大小: %zu 字节\n", config.max_size);
    printf("  最大备份数: %d\n", config.max_backups);
    printf("  压缩备份: %s\n", config.compress ? "是" : "否");
    printf("  创建文件: %s\n", config.create_file ? "是" : "否");
    printf("  详细模式: %s\n", config.verbose ? "是" : "否");
    printf("  检查大小: %s\n", config.check_size ? "是" : "否");
    printf("  文件权限: %o\n", config.file_mode);
    printf("  滚动间隔: %d 秒\n", config.rotate_interval);
    printf("  强制滚动: %s\n", config.force_rotate ? "是" : "否");
}

// 演示 3: 触发类型
static void demo_triggers(void) {
    printf("\n=== 演示 3: 滚动触发类型 ===\n");

    printf("日志滚动触发条件:\n\n");

    printf("1. SIZE (按大小)\n");
    printf("   - 当日志文件超过 max_size 时触发\n");
    printf("   - 最常用的触发方式\n\n");

    printf("2. TIME (按时间)\n");
    printf("   - 按 rotate_interval 设置的时间间隔触发\n");
    printf("   - 适合定期归档\n\n");

    printf("3. FORCE (强制)\n");
    printf("   - 无视条件强制滚动\n");
    printf("   - 适合手动触发\n\n");

    printf("4. SIGNAL (信号)\n");
    printf("   - 通过信号触发\n");
    printf("   - 适合外部控制\n");
}

// 演示 4: 检查是否需要滚动
static void demo_check_rotation(void) {
    printf("\n=== 演示 4: 检查是否需要滚动 ===\n");

    const char *logfile = "/tmp/demo_rotate2.log";

    // 创建小文件
    FILE *fp = fopen(logfile, "w");
    if (fp) {
        fprintf(fp, "Small log content\n");
        fclose(fp);
    }

    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_size = 1024;  // 1KB

    log_rotate_error_t error;
    bool needs = log_rotate_needs_rotation(logfile, &config, &error);

    printf("文件大小检查:\n");
    printf("  最大允许: %zu 字节\n", config.max_size);
    printf("  是否需要滚动: %s\n", needs ? "是" : "否");
}

// 演示 5: 清理旧日志
static void demo_cleanup(void) {
    printf("\n=== 演示 5: 清理旧日志 ===\n");

    const char *logfile = "/tmp/demo_rotate.log";

    printf("清理旧日志文件 (保留 2 个备份)...\n");

    log_rotate_error_t error;
    log_rotate_cleanup(logfile, 2, &error);

    if (error == LOG_ROTATE_OK) {
        printf("清理完成\n");
    } else {
        printf("清理结果: %d\n", error);
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. Web 服务器日志\n");
    printf("   - 按天滚动 access.log\n");
    printf("   - 保留最近 30 天\n");
    printf("\n");

    printf("2. 应用日志\n");
    printf("   - 按大小滚动 (如 100MB)\n");
    printf("   - 保留最近 10 个备份\n");
    printf("\n");

    printf("3. 系统日志\n");
    printf("   - 配合 logrotate 工具\n");
    printf("   - 定期归档和清理\n");
    printf("\n");

    printf("4. 数据库日志\n");
    printf("   - 事务日志管理\n");
    printf("   - 防止磁盘空间耗尽\n");
}

int main(void) {
    printf("========================================\n");
    printf("    日志滚动演示\n");
    printf("========================================\n");

    demo_basic();
    demo_config();
    demo_triggers();
    demo_check_rotation();
    demo_cleanup();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
