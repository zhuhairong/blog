/**
 * 日志系统演示程序
 *
 * 功能：
 * - 不同日志级别
 * - 日志输出控制
 * - 文件日志记录
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/log.h"

// 演示 1: 日志级别
static void demo_levels(void) {
    printf("\n=== 演示 1: 日志级别 ===\n");

    printf("日志级别 (从低到高):\n");
    printf("  TRACE - 最详细的跟踪信息\n");
    printf("  DEBUG - 调试信息\n");
    printf("  INFO  - 一般信息\n");
    printf("  WARN  - 警告信息\n");
    printf("  ERROR - 错误信息\n");
    printf("  FATAL - 致命错误\n\n");

    printf("输出各级别日志:\n");
    LOGT("这是一条 TRACE 日志");
    LOGD("这是一条 DEBUG 日志");
    LOGI("这是一条 INFO 日志");
    LOGW("这是一条 WARN 日志");
    LOGE("这是一条 ERROR 日志");
    LOGF("这是一条 FATAL 日志");
}

// 演示 2: 日志级别控制
static void demo_level_control(void) {
    printf("\n=== 演示 2: 日志级别控制 ===\n");

    printf("当前日志级别: INFO\n");
    log_set_level(LOG_LEVEL_INFO);

    printf("\n设置级别为 INFO 后:\n");
    LOGT("TRACE - 不会显示 (低于 INFO)");
    LOGD("DEBUG - 不会显示 (低于 INFO)");
    LOGI("INFO  - 会显示");
    LOGW("WARN  - 会显示");
    LOGE("ERROR - 会显示");

    printf("\n设置级别为 WARN:\n");
    log_set_level(LOG_LEVEL_WARN);
    LOGI("INFO  - 不会显示");
    LOGW("WARN  - 会显示");
    LOGE("ERROR - 会显示");

    // 恢复默认
    log_set_level(LOG_LEVEL_DEBUG);
}

// 演示 3: 安静模式
static void demo_quiet(void) {
    printf("\n=== 演示 3: 安静模式 ===\n");

    printf("启用安静模式 (quiet mode):\n");
    log_set_quiet(1);

    LOGI("这条日志不会输出到控制台");
    LOGW("这条也不会");

    printf("安静模式已启用，上面的日志被抑制\n");

    log_set_quiet(0);
    printf("已关闭安静模式\n");
}

// 演示 4: 文件日志
static void demo_file_log(void) {
    printf("\n=== 演示 4: 文件日志 ===\n");

    const char *logfile = "/tmp/demo_log.txt";
    FILE *fp = fopen(logfile, "w");

    if (fp) {
        printf("添加文件输出: %s\n", logfile);
        log_add_fp(fp, LOG_LEVEL_DEBUG);

        LOGI("这条日志会同时输出到控制台和文件");
        LOGD("DEBUG 信息也会写入文件");

        fclose(fp);
        printf("日志已写入文件\n");
    } else {
        printf("无法创建日志文件\n");
    }
}

// 演示 5: 格式化日志
static void demo_formatting(void) {
    printf("\n=== 演示 5: 格式化日志 ===\n");

    int count = 42;
    double value = 3.14159;
    const char *name = "test";

    LOGI("整数: %d", count);
    LOGI("浮点数: %.2f", value);
    LOGI("字符串: %s", name);
    LOGI("混合: %s = %d, pi = %.5f", name, count, value);
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 应用程序调试\n");
    printf("   - 跟踪程序执行流程\n");
    printf("   - 记录变量值\n");
    printf("\n");

    printf("2. 错误追踪\n");
    printf("   - 记录错误发生位置\n");
    printf("   - 错误上下文信息\n");
    printf("\n");

    printf("3. 性能监控\n");
    printf("   - 记录执行时间\n");
    printf("   - 资源使用情况\n");
    printf("\n");

    printf("4. 审计日志\n");
    printf("   - 用户操作记录\n");
    printf("   - 系统事件记录\n");
}

int main(void) {
    printf("========================================\n");
    printf("    日志系统演示\n");
    printf("========================================\n");

    // 设置初始日志级别
    log_set_level(LOG_LEVEL_DEBUG);
    log_set_quiet(0);

    demo_levels();
    demo_level_control();
    demo_quiet();
    demo_file_log();
    demo_formatting();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
