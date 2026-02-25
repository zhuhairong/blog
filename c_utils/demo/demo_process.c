/**
 * 进程管理演示程序
 *
 * 功能：
 * - 执行命令
 * - 捕获输出
 * - 超时控制
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/process.h"

// 演示 1: 基本命令执行概念
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本命令执行 ===\n");

    printf("process_exec 函数:\n");
    printf("  功能: 执行命令并捕获输出\n");
    printf("  参数: cmd - 命令字符串\n");
    printf("        exit_code - 退出码输出\n");
    printf("  返回: 命令输出字符串\n\n");

    printf("示例用法:\n");
    printf("  int exit_code;\n");
    printf("  char *output = process_exec(\"echo hello\", &exit_code);\n");
    printf("  printf(\"%%s\", output);\n");
    printf("  free(output);\n");
}

// 演示 2: process_run 函数
static void demo_run(void) {
    printf("\n=== 演示 2: process_run 函数 ===\n");

    printf("process_run 函数:\n");
    printf("  功能: 执行命令并等待完成\n");
    printf("  参数: cmd - 命令字符串\n");
    printf("  返回: 退出码\n\n");

    printf("示例用法:\n");
    printf("  int code = process_run(\"ls -la\");\n");
    printf("  if (code == 0) {\n");
    printf("      printf(\"成功\\n\");\n");
    printf("  }\n");
}

// 演示 3: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 3: 进程配置选项 ===\n");

    printf("process_config_t 结构:\n");
    printf("  capture_stdout: 捕获标准输出\n");
    printf("  capture_stderr: 捕获标准错误\n");
    printf("  merge_stderr: 合并标准错误到标准输出\n");
    printf("  timeout_ms: 超时时间(毫秒)\n");
    printf("  max_output_size: 最大输出大小\n");
    printf("  working_dir: 工作目录\n\n");

    printf("process_result_t 结构:\n");
    printf("  exit_code: 退出码\n");
    printf("  stdout: 标准输出\n");
    printf("  stdout_len: 标准输出长度\n");
    printf("  stderr: 标准错误\n");
    printf("  stderr_len: 标准错误长度\n");
    printf("  timed_out: 是否超时\n");
    printf("  succeeded: 是否成功\n");
}

// 演示 4: 详细执行结果
static void demo_exec_ex(void) {
    printf("\n=== 演示 4: 详细执行结果 ===\n");

    printf("process_exec_ex 函数:\n");
    printf("  功能: 执行命令并获取详细结果\n");
    printf("  参数: cmd - 命令字符串\n");
    printf("        config - 配置选项\n");
    printf("        result - 结果输出\n");
    printf("        error - 错误码输出\n");
    printf("  返回: 是否成功\n\n");

    printf("示例用法:\n");
    printf("  process_config_t config = process_default_config();\n");
    printf("  config.capture_stdout = true;\n");
    printf("  config.timeout_ms = 5000;\n\n");
    printf("  process_result_t result;\n");
    printf("  process_error_t error;\n\n");
    printf("  if (process_exec_ex(\"uname -a\", &config, &result, &error)) {\n");
    printf("      printf(\"输出: %%s\\n\", result.stdout);\n");
    printf("      free(result.stdout);\n");
    printf("  }\n");
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  PROCESS_OK - 成功\n");
    printf("  PROCESS_ERROR_NULL_PTR - 空指针\n");
    printf("  PROCESS_ERROR_FORK_FAILED - 进程创建失败\n");
    printf("  PROCESS_ERROR_PIPE_FAILED - 管道创建失败\n");
    printf("  PROCESS_ERROR_EXEC_FAILED - 命令执行失败\n");
    printf("  PROCESS_ERROR_WAIT_FAILED - 等待进程失败\n");
    printf("  PROCESS_ERROR_MEMORY_ALLOC - 内存分配失败\n");
    printf("  PROCESS_ERROR_TIMEOUT - 超时\n");

    printf("\n常见错误场景:\n");
    printf("  - 命令不存在 -> EXEC_FAILED\n");
    printf("  - 权限不足 -> EXEC_FAILED\n");
    printf("  - 执行超时 -> TIMEOUT\n");
}

// 演示 6: 超时控制
static void demo_timeout(void) {
    printf("\n=== 演示 6: 超时控制 ===\n");

    printf("超时控制说明:\n");
    printf("  - 设置 timeout_ms 限制执行时间\n");
    printf("  - 超时后自动终止进程\n");
    printf("  - result.timed_out 标记超时状态\n");
    printf("  - 防止僵尸进程和资源泄漏\n\n");

    printf("示例:\n");
    printf("  config.timeout_ms = 1000;  // 1秒超时\n");
    printf("  // 执行可能长时间运行的命令\n");
    printf("  // 如果超过1秒，自动终止\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 系统管理工具\n");
    printf("   - 执行系统命令\n");
    printf("   - 获取系统信息\n");
    printf("   - 批量操作\n\n");

    printf("2. 构建系统\n");
    printf("   - 编译代码\n");
    printf("   - 运行测试\n");
    printf("   - 捕获输出\n\n");

    printf("3. 自动化脚本\n");
    printf("   - 定时任务\n");
    printf("   - 批处理\n");
    printf("   - 流程编排\n\n");

    printf("4. 监控工具\n");
    printf("   - 执行检查命令\n");
    printf("   - 解析输出\n");
    printf("   - 告警触发\n\n");

    printf("5. 开发工具\n");
    printf("   - 代码格式化\n");
    printf("   - 静态检查\n");
    printf("   - 版本控制\n");
}

int main(void) {
    printf("========================================\n");
    printf("    进程管理演示\n");
    printf("========================================\n");

    demo_basic();
    demo_run();
    demo_config();
    demo_exec_ex();
    demo_errors();
    demo_timeout();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
