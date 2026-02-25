/**
 * @file demo_cron.c
 * @brief Cron 表达式解析演示
 *
 * 本演示展示了 Cron 表达式的解析和匹配，包括：
 * - Cron 表达式解析
 * - 时间匹配
 * - 下次执行时间计算
 * - 常见表达式示例
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/cron.h"

// 打印 Cron 表达式字段
static void print_cron_field(const char *name, bool *field, int max) {
    printf("  %s: ", name);
    int count = 0;
    for (int i = 0; i < max; i++) {
        if (field[i]) {
            if (count > 0) printf(",");
            printf("%d", i);
            count++;
            if (count > 10) {
                printf(",...");
                break;
            }
        }
    }
    if (count == 0) printf("*");
    printf("\n");
}

// 演示 1: 基本 Cron 表达式解析
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 Cron 表达式解析 ===\n");

    const char *expressions[] = {
        "* * * * *",           // 每分钟
        "0 * * * *",           // 每小时
        "0 0 * * *",           // 每天
        "0 0 * * 0",           // 每周日
        "0 0 1 * *",           // 每月1日
        "0 0 1 1 *",           // 每年1月1日
        "*/5 * * * *",         // 每5分钟
        "0 9-17 * * 1-5",      // 工作日 9-17点
        "0 0,12 * * *",        // 每天 0点和12点
        "30 2 * * 1-5"         // 工作日 2:30
    };
    int num_exprs = sizeof(expressions) / sizeof(expressions[0]);

    printf("Cron 表达式解析:\n\n");
    for (int i = 0; i < num_exprs; i++) {
        cron_expr_t cron;
        printf("表达式: %s\n", expressions[i]);
        
        if (cron_parse(expressions[i], &cron)) {
            print_cron_field("分", cron.minute, 60);
            print_cron_field("时", cron.hour, 24);
            print_cron_field("日", cron.day_of_month, 32);
            print_cron_field("月", cron.month, 13);
            print_cron_field("周", cron.day_of_week, 7);
        } else {
            printf("  解析失败: %s\n", cron.error_msg);
        }
        printf("\n");
    }
}

// 演示 2: 时间匹配
static void demo_match(void) {
    printf("\n=== 演示 2: 时间匹配 ===\n");

    // 创建测试时间
    struct tm test_time = {
        .tm_year = 2024 - 1900,
        .tm_mon = 5,  // 6月
        .tm_mday = 15,
        .tm_hour = 14,
        .tm_min = 30,
        .tm_wday = 6  // 周六
    };

    printf("测试时间: 2024-06-15 14:30 (周六)\n\n");

    const char *expressions[] = {
        "* * * * *",
        "30 * * * *",
        "30 14 * * *",
        "30 14 15 * *",
        "30 14 * 6 *",
        "30 14 * * 6",
        "0 14 * * *",
        "30 15 * * *"
    };
    int num_exprs = sizeof(expressions) / sizeof(expressions[0]);

    printf("匹配测试:\n");
    for (int i = 0; i < num_exprs; i++) {
        cron_expr_t cron;
        cron_parse(expressions[i], &cron);
        
        bool matches = cron_match(&cron, &test_time);
        printf("  %-20s %s\n", expressions[i], matches ? "✓ 匹配" : "✗ 不匹配");
    }
}

// 演示 3: 下次执行时间
static void demo_next(void) {
    printf("\n=== 演示 3: 下次执行时间 ===\n");

    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    
    printf("当前时间: %s", asctime(tm_now));

    const char *expressions[] = {
        "* * * * *",
        "*/5 * * * *",
        "0 * * * *",
        "0 0 * * *",
        "0 9 * * 1-5"
    };
    int num_exprs = sizeof(expressions) / sizeof(expressions[0]);

    printf("\n下次执行时间:\n");
    for (int i = 0; i < num_exprs; i++) {
        cron_expr_t cron;
        cron_parse(expressions[i], &cron);
        
        struct tm next;
        cron_error_t error;
        
        if (cron_next(&cron, tm_now, &next, &error)) {
            printf("  %-20s %s", expressions[i], asctime(&next));
        } else {
            printf("  %-20s 计算失败\n", expressions[i]);
        }
    }
}

// 演示 4: 特殊表达式
static void demo_special(void) {
    printf("\n=== 演示 4: 特殊表达式 ===\n");

    printf("预定义宏:\n");
    printf("  @yearly  (或 @annually)  每年 1月1日 0:00\n");
    printf("  @monthly                 每月 1日 0:00\n");
    printf("  @weekly                  每周日 0:00\n");
    printf("  @daily   (或 @midnight)  每天 0:00\n");
    printf("  @hourly                  每小时 0分\n");
    printf("  @reboot                  系统启动时\n");

    printf("\n特殊字符:\n");
    printf("  *     任意值\n");
    printf("  ,     列表分隔符 (如: 1,3,5)\n");
    printf("  -     范围 (如: 1-5)\n");
    printf("  /     步长 (如: */5)\n");
    printf("  L     最后 (仅用于日和周)\n");
    printf("  #     第N个 (如: 2#1 第一个周一)\n");
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CRON_OK (%d): 成功\n", CRON_OK);
    printf("  CRON_ERROR_INVALID_EXPRESSION (%d): 无效表达式\n", CRON_ERROR_INVALID_EXPRESSION);
    printf("  CRON_ERROR_INVALID_FIELD (%d): 无效字段\n", CRON_ERROR_INVALID_FIELD);
    printf("  CRON_ERROR_INVALID_RANGE (%d): 无效范围\n", CRON_ERROR_INVALID_RANGE);
    printf("  CRON_ERROR_INVALID_STEP (%d): 无效步长\n", CRON_ERROR_INVALID_STEP);
    printf("  CRON_ERROR_UNSUPPORTED_FEATURE (%d): 不支持的功能\n", CRON_ERROR_UNSUPPORTED_FEATURE);
    printf("  CRON_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CRON_ERROR_MEMORY_ALLOC);

    printf("\n无效表达式测试:\n");
    const char *invalid_exprs[] = {
        "",
        "* * *",
        "60 * * * *",
        "* 25 * * *",
        "* * 32 * *",
        "* * * 13 *",
        "* * * * 8",
        "abc * * * *"
    };
    int num_invalid = sizeof(invalid_exprs) / sizeof(invalid_exprs[0]);

    for (int i = 0; i < num_invalid; i++) {
        cron_expr_t cron;
        bool result = cron_parse(invalid_exprs[i], &cron);
        printf("  \"%s\": %s\n", 
               invalid_exprs[i][0] ? invalid_exprs[i] : "(空)",
               result ? "解析成功" : "解析失败");
    }
}

// 演示 6: 实际应用示例
static void demo_examples(void) {
    printf("\n=== 演示 6: 实际应用示例 ===\n");

    printf("常见定时任务:\n\n");

    printf("1. 系统维护:\n");
    printf("   0 2 * * *     每天 2:00 备份\n");
    printf("   0 3 * * 0     每周日 3:00 清理日志\n");
    printf("   0 4 1 * *     每月 1日 4:00 生成报告\n\n");

    printf("2. 业务任务:\n");
    printf("   */5 * * * *   每 5 分钟检查队列\n");
    printf("   0 */2 * * *   每 2 小时同步数据\n");
    printf("   0 9-18 * * 1-5 工作日每小时检查\n\n");

    printf("3. 监控告警:\n");
    printf("   */1 * * * *   每分钟检查服务状态\n");
    printf("   0 */6 * * *   每 6 小时发送统计\n");
    printf("   0 0 * * *     每天发送日报\n\n");

    printf("4. 数据处理:\n");
    printf("   0 1 * * *     每天 1:00 ETL 任务\n");
    printf("   0 */4 * * *   每 4 小时聚合数据\n");
    printf("   0 2 * * 0     每周重建索引\n");
}

// 演示 7: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 7: 性能测试 ===\n");

    const char *expr = "*/5 * * * *";
    const int iterations = 100000;

    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        cron_expr_t cron;
        cron_parse(expr, &cron);
    }
    
    clock_t end = clock();
    double time_ms = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("解析 %d 次表达式 \"%s\"\n", iterations, expr);
    printf("总时间: %.3f ms\n", time_ms);
    printf("平均每次: %.6f ms\n", time_ms / iterations);
}

// 演示 8: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("Cron 表达式的应用场景:\n\n");

    printf("1. 任务调度系统:\n");
    printf("   - 定时任务执行\n");
    printf("   - 批量作业调度\n");
    printf("   - 工作流编排\n\n");

    printf("2. 监控系统:\n");
    printf("   - 定时采集指标\n");
    printf("   - 健康检查\n");
    printf("   - 告警触发\n\n");

    printf("3. 数据处理:\n");
    printf("   - ETL 任务调度\n");
    printf("   - 数据归档\n");
    printf("   - 报表生成\n\n");

    printf("4. 系统运维:\n");
    printf("   - 日志轮转\n");
    printf("   - 备份任务\n");
    printf("   - 清理任务\n");
}

// 演示 9: 与其他调度方式比较
static void demo_comparison(void) {
    printf("\n=== 演示 9: 与其他调度方式比较 ===\n");

    printf("调度方式比较:\n\n");

    printf("特性          Cron        间隔调度    事件驱动\n");
    printf("-------------------------------------------\n");
    printf("精确时间      ✓           ✗           ✗\n");
    printf("简单重复      ✓           ✓           ✗\n");
    printf("复杂模式      ✓           ✗           ✓\n");
    printf("实时响应      ✗           ✗           ✓\n");
    printf("可读性        高          中          低\n\n");

    printf("Cron 优势:\n");
    printf("  - 表达能力强\n");
    printf("  - 业界标准\n");
    printf("  - 易于理解\n");
    printf("  - 无需编程\n");
}

// 演示 10: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 10: 最佳实践 ===\n");

    printf("Cron 表达式最佳实践:\n\n");

    printf("1. 时间选择:\n");
    printf("   - 避开高峰时段\n");
    printf("   - 分散任务时间\n");
    printf("   - 考虑时区问题\n\n");

    printf("2. 表达式编写:\n");
    printf("   - 使用注释说明用途\n");
    printf("   - 避免过于复杂的表达式\n");
    printf("   - 测试边界情况\n\n");

    printf("3. 任务设计:\n");
    printf("   - 任务幂等性\n");
    printf("   - 超时处理\n");
    printf("   - 错误重试\n\n");

    printf("4. 监控运维:\n");
    printf("   - 记录执行日志\n");
    printf("   - 监控执行时间\n");
    printf("   - 设置告警阈值\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Cron 表达式解析演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_match();
    demo_next();
    demo_special();
    demo_errors();
    demo_examples();
    demo_performance();
    demo_use_cases();
    demo_comparison();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
