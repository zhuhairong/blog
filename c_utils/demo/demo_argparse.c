/**
 * @file demo_argparse.c
 * @brief 命令行参数解析器演示
 *
 * 本演示展示了命令行参数解析器的各种用法，包括：
 * - 基本选项解析（布尔、整数、字符串、浮点数）
 * - 必需选项和可选选项
 * - 默认值处理
 * - 位置参数
 * - 帮助信息生成
 *
 * 编译后可以使用以下方式运行：
 *   ./demo_argparse -h
 *   ./demo_argparse -v -n 42 -s "hello" -r 3.14 input.txt
 *   ./demo_argparse --verbose --count 10 --file data.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/argparse.h"

// 打印解析结果
static void print_results(const argparse_t* ap, bool verbose, int count,
                          const char* filename, double ratio, int argc, const char** argv) {
    printf("\n解析结果:\n");
    printf("  详细模式 (-v): %s\n", verbose ? "开启" : "关闭");
    printf("  计数 (-n): %d\n", count);
    printf("  文件名 (-s): %s\n", filename ? filename : "(未指定)");
    printf("  比率 (-r): %.4f\n", ratio);

    printf("\n选项设置状态:\n");
    printf("  verbose 被设置: %s\n", argparse_is_set(ap, "verbose") ? "是" : "否");
    printf("  count 被设置: %s\n", argparse_is_set(ap, "count") ? "是" : "否");
    printf("  file 被设置: %s\n", argparse_is_set(ap, "file") ? "是" : "否");
    printf("  ratio 被设置: %s\n", argparse_is_set(ap, "ratio") ? "是" : "否");

    printf("\n位置参数:\n");
    for (int i = 0; i < argc; i++) {
        printf("  [%d]: %s\n", i, argv[i]);
    }
}

// 演示 1: 基本用法
static int demo_basic(int argc, char** argv) {
    printf("\n=== 演示 1: 基本参数解析 ===\n");

    argparse_t* ap = argparse_create(
        "基本参数解析演示",
        "demo_argparse [选项] [文件...]"
    );
    if (!ap) {
        fprintf(stderr, "创建解析器失败\n");
        return 1;
    }

    // 定义变量接收解析结果
    bool verbose = false;
    int count = 0;
    const char* filename = NULL;
    double ratio = 1.0;

    // 添加选项
    argparse_add_bool(ap, 'v', "verbose", "启用详细输出", &verbose, false);
    argparse_add_int(ap, 'n', "count", "设置计数", &count, 10, false);
    argparse_add_string(ap, 's', "file", "指定文件名", &filename, "default.txt", false);
    argparse_add_double(ap, 'r', "ratio", "设置比率", &ratio, 1.0, false);

    // 解析参数
    int result = argparse_parse(ap, argc, argv);
    if (result == 0) {
        print_results(ap, verbose, count, filename, ratio,
                      ap->positional_count, ap->positional_args);
    }

    argparse_destroy(ap);
    return result;
}

// 演示 2: 带必需选项
static int demo_required(int argc, char** argv) {
    printf("\n=== 演示 2: 必需选项 ===\n");

    argparse_t* ap = argparse_create(
        "必需选项演示",
        "demo_argparse --input <文件> --output <文件>"
    );
    if (!ap) {
        fprintf(stderr, "创建解析器失败\n");
        return 1;
    }

    const char* input_file = NULL;
    const char* output_file = NULL;
    int threads = 4;

    // 添加必需选项
    argparse_add_string(ap, 'i', "input", "输入文件（必需）", &input_file, NULL, true);
    argparse_add_string(ap, 'o', "output", "输出文件（必需）", &output_file, NULL, true);
    argparse_add_int(ap, 't', "threads", "线程数", &threads, 4, false);

    int result = argparse_parse(ap, argc, argv);
    if (result == 0) {
        printf("解析成功:\n");
        printf("  输入文件: %s\n", input_file);
        printf("  输出文件: %s\n", output_file);
        printf("  线程数: %d\n", threads);
    }

    argparse_destroy(ap);
    return result;
}

// 演示 3: 文件处理工具示例
static int demo_file_processor(int argc, char** argv) {
    printf("\n=== 演示 3: 文件处理工具 ===\n");

    argparse_t* ap = argparse_create(
        "文件处理工具 - 演示 argparse 在实际应用中的使用",
        "demo_argparse [选项] <输入文件> [输出文件]"
    );
    if (!ap) {
        fprintf(stderr, "创建解析器失败\n");
        return 1;
    }

    bool verbose = false;
    bool dry_run = false;
    int compression = 6;
    const char* format = "auto";
    double quality = 0.9;

    argparse_add_bool(ap, 'v', "verbose", "显示详细处理信息", &verbose, false);
    argparse_add_bool(ap, 'd', "dry-run", "模拟运行，不实际执行", &dry_run, false);
    argparse_add_int(ap, 'c', "compression", "压缩级别 (0-9)", &compression, 6, false);
    argparse_add_string(ap, 'f', "format", "输出格式", &format, "auto", false);
    argparse_add_double(ap, 'q', "quality", "输出质量 (0.0-1.0)", &quality, 0.9, false);

    int result = argparse_parse(ap, argc, argv);
    if (result == 0) {
        printf("文件处理配置:\n");
        printf("  详细模式: %s\n", verbose ? "开启" : "关闭");
        printf("  模拟运行: %s\n", dry_run ? "是" : "否");
        printf("  压缩级别: %d\n", compression);
        printf("  输出格式: %s\n", format);
        printf("  输出质量: %.2f\n", quality);

        if (ap->positional_count > 0) {
            printf("  输入文件: %s\n", ap->positional_args[0]);
            if (ap->positional_count > 1) {
                printf("  输出文件: %s\n", ap->positional_args[1]);
            }
        }

        // 模拟处理
        if (!dry_run) {
            printf("\n开始处理...\n");
            if (verbose) {
                printf("  读取输入文件...\n");
                printf("  应用压缩 (级别 %d)...\n", compression);
                printf("  设置输出格式为 %s...\n", format);
                printf("  调整质量为 %.2f...\n", quality);
            }
            printf("处理完成!\n");
        } else {
            printf("\n[模拟运行模式，未实际执行]\n");
        }
    }

    argparse_destroy(ap);
    return result;
}

// 演示 4: 网络工具示例
static int demo_network_tool(int argc, char** argv) {
    printf("\n=== 演示 4: 网络工具 ===\n");

    argparse_t* ap = argparse_create(
        "网络诊断工具 - ping/traceroute 风格",
        "demo_argparse [选项] <主机名或IP>"
    );
    if (!ap) {
        fprintf(stderr, "创建解析器失败\n");
        return 1;
    }

    int count = 4;
    int timeout = 5;
    int interval = 1;
    int port = 80;
    bool ipv6 = false;
    bool continuous = false;

    argparse_add_int(ap, 'c', "count", "发送包的数量", &count, 4, false);
    argparse_add_int(ap, 't', "timeout", "超时时间（秒）", &timeout, 5, false);
    argparse_add_int(ap, 'i', "interval", "包间隔（秒）", &interval, 1, false);
    argparse_add_int(ap, 'p', "port", "目标端口", &port, 80, false);
    argparse_add_bool(ap, '6', "ipv6", "使用 IPv6", &ipv6, false);
    argparse_add_bool(ap, 'C', "continuous", "持续发送", &continuous, false);

    int result = argparse_parse(ap, argc, argv);
    if (result == 0) {
        if (ap->positional_count == 0) {
            printf("错误: 需要指定目标主机\n");
            argparse_usage(ap);
            argparse_destroy(ap);
            return 1;
        }

        printf("网络诊断配置:\n");
        printf("  目标主机: %s\n", ap->positional_args[0]);
        printf("  协议: %s\n", ipv6 ? "IPv6" : "IPv4");
        printf("  目标端口: %d\n", port);
        printf("  包数量: %s\n", continuous ? "无限" : "有限");
        if (!continuous) {
            printf("  发送次数: %d\n", count);
        }
        printf("  超时时间: %d 秒\n", timeout);
        printf("  发送间隔: %d 秒\n", interval);

        printf("\n模拟网络诊断...\n");
        int max_count = continuous ? 10 : count;  // 演示限制
        for (int i = 0; i < max_count; i++) {
            printf("  包 %d: 发送到 %s... 回复来自 %s: 时间=%dms\n",
                   i + 1, ap->positional_args[0], ap->positional_args[0],
                   20 + i * 2);
        }
        if (continuous) {
            printf("  ... (持续模式，按 Ctrl+C 停止)\n");
        }
    }

    argparse_destroy(ap);
    return result;
}

// 演示 5: 配置工具示例
static int demo_config_tool(int argc, char** argv) {
    printf("\n=== 演示 5: 配置工具 ===\n");

    argparse_t* ap = argparse_create(
        "应用程序配置工具",
        "demo_argparse [选项] [命令]"
    );
    if (!ap) {
        fprintf(stderr, "创建解析器失败\n");
        return 1;
    }

    const char* config_file = "~/.config/myapp.conf";
    const char* log_level = "info";
    int max_connections = 100;
    double cache_ratio = 0.25;
    bool enable_ssl = false;
    bool debug_mode = false;

    argparse_add_string(ap, 'c', "config", "配置文件路径", &config_file, "~/.config/myapp.conf", false);
    argparse_add_string(ap, 'l', "log-level", "日志级别 (debug|info|warn|error)", &log_level, "info", false);
    argparse_add_int(ap, 'm', "max-connections", "最大连接数", &max_connections, 100, false);
    argparse_add_double(ap, 'r', "cache-ratio", "缓存比例", &cache_ratio, 0.25, false);
    argparse_add_bool(ap, 's', "enable-ssl", "启用 SSL", &enable_ssl, false);
    argparse_add_bool(ap, 'd', "debug", "调试模式", &debug_mode, false);

    int result = argparse_parse(ap, argc, argv);
    if (result == 0) {
        printf("应用程序配置:\n");
        printf("  配置文件: %s\n", config_file);
        printf("  日志级别: %s\n", log_level);
        printf("  最大连接数: %d\n", max_connections);
        printf("  缓存比例: %.2f\n", cache_ratio);
        printf("  SSL 启用: %s\n", enable_ssl ? "是" : "否");
        printf("  调试模式: %s\n", debug_mode ? "是" : "否");

        // 处理命令
        if (ap->positional_count > 0) {
            const char* command = ap->positional_args[0];
            printf("\n执行命令: %s\n", command);

            if (strcmp(command, "start") == 0) {
                printf("  启动服务...\n");
                printf("  加载配置: %s\n", config_file);
                printf("  设置日志级别: %s\n", log_level);
                printf("  服务已启动!\n");
            } else if (strcmp(command, "stop") == 0) {
                printf("  停止服务...\n");
                printf("  服务已停止!\n");
            } else if (strcmp(command, "status") == 0) {
                printf("  服务状态: 运行中\n");
                printf("  当前连接数: %d/%d\n", 42, max_connections);
                printf("  缓存使用率: %.1f%%\n", cache_ratio * 100);
            } else {
                printf("  未知命令: %s\n", command);
                printf("  可用命令: start, stop, status\n");
            }
        } else {
            printf("\n提示: 使用 'start', 'stop', 或 'status' 命令\n");
        }
    }

    argparse_destroy(ap);
    return result;
}

int main(int argc, char** argv) {
    printf("========================================\n");
    printf("    命令行参数解析器演示程序\n");
    printf("========================================\n");

    if (argc < 2) {
        printf("\n用法: %s <演示编号> [选项]\n", argv[0]);
        printf("\n可用演示:\n");
        printf("  1 - 基本参数解析\n");
        printf("  2 - 必需选项\n");
        printf("  3 - 文件处理工具\n");
        printf("  4 - 网络工具\n");
        printf("  5 - 配置工具\n");
        printf("\n示例:\n");
        printf("  %s 1 -v -n 42\n", argv[0]);
        printf("  %s 2 -i input.txt -o output.txt\n", argv[0]);
        printf("  %s 3 -v --compression 9 file.txt\n", argv[0]);
        printf("  %s 4 -c 10 google.com\n", argv[0]);
        printf("  %s 5 -d start\n", argv[0]);
        return 1;
    }

    int demo_num = atoi(argv[1]);

    // 移除演示编号参数
    int new_argc = argc - 1;
    char** new_argv = argv + 1;
    new_argv[0] = argv[0];  // 保留程序名

    int result = 0;
    switch (demo_num) {
        case 1:
            result = demo_basic(new_argc, new_argv);
            break;
        case 2:
            result = demo_required(new_argc, new_argv);
            break;
        case 3:
            result = demo_file_processor(new_argc, new_argv);
            break;
        case 4:
            result = demo_network_tool(new_argc, new_argv);
            break;
        case 5:
            result = demo_config_tool(new_argc, new_argv);
            break;
        default:
            fprintf(stderr, "无效的演示编号: %d\n", demo_num);
            return 1;
    }

    return result;
}
