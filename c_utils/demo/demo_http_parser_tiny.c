/**
 * 极简 HTTP 解析器演示程序
 *
 * 功能：
 * - 请求行解析
 * - 方法验证
 * - 版本验证
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/http_parser_tiny.h"

// 演示 1: 解析请求行
static void demo_parse_request_line(void) {
    printf("\n=== 演示 1: 请求行解析 ===\n");

    struct {
        const char *request;
        const char *description;
    } cases[] = {
        {"GET /index.html HTTP/1.1\r\n", "标准 GET 请求"},
        {"POST /api/users HTTP/1.1\r\n", "POST 请求"},
        {"PUT /resource/123 HTTP/1.0\r\n", "PUT 请求"},
        {"DELETE /item/456 HTTP/1.1\r\n", "DELETE 请求"},
        {"GET /path?key=value HTTP/1.1\r\n", "带查询参数"},
        {NULL, NULL}
    };

    for (int i = 0; cases[i].request; i++) {
        printf("\n%s:\n", cases[i].description);
        printf("  请求: %s", cases[i].request);

        http_req_t req;
        http_parser_tiny_error_t error = http_parse_request_line(
            cases[i].request, strlen(cases[i].request), &req);

        if (error == HTTP_PARSER_TINY_OK) {
            printf("  方法: %s\n", req.method);
            printf("  路径: %s\n", req.path);
            printf("  版本: %s\n", req.version);
        } else {
            printf("  解析失败: %d\n", error);
        }
    }
}

// 演示 2: 方法验证
static void demo_validate_method(void) {
    printf("\n=== 演示 2: HTTP 方法验证 ===\n");

    const char *methods[] = {
        "GET", "POST", "PUT", "DELETE", "HEAD",
        "OPTIONS", "PATCH", "TRACE", "CONNECT",
        "INVALID", "get", "", NULL
    };

    printf("方法验证:\n");
    for (int i = 0; methods[i]; i++) {
        bool valid = http_method_is_valid(methods[i]);
        printf("  \"%s\" -> %s\n", methods[i], valid ? "有效" : "无效");
    }
}

// 演示 3: 版本验证
static void demo_validate_version(void) {
    printf("\n=== 演示 3: HTTP 版本验证 ===\n");

    const char *versions[] = {
        "HTTP/1.0", "HTTP/1.1", "HTTP/2.0",
        "HTTP/1.2", "http/1.1", "HTTP/3",
        "", "1.1", NULL
    };

    printf("版本验证:\n");
    for (int i = 0; versions[i]; i++) {
        bool valid = http_version_is_valid(versions[i]);
        printf("  \"%s\" -> %s\n", versions[i], valid ? "有效" : "无效");
    }
}

// 演示 4: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 4: 错误处理 ===\n");

    struct {
        const char *request;
        const char *description;
    } error_cases[] = {
        {"", "空请求"},
        {"GET", "缺少路径和版本"},
        {"GET /path", "缺少版本"},
        {"INVALID /path HTTP/1.1\r\n", "无效方法"},
        {"GET /path HTTP/2.0\r\n", "无效版本"},
        {"GET/path HTTP/1.1\r\n", "缺少空格"},
        {NULL, NULL}
    };

    printf("错误案例:\n");
    for (int i = 0; error_cases[i].request; i++) {
        http_req_t req;
        http_parser_tiny_error_t error = http_parse_request_line(
            error_cases[i].request, strlen(error_cases[i].request), &req);

        printf("\n%s:\n", error_cases[i].description);
        printf("  输入: \"%s\"\n", error_cases[i].request);
        printf("  错误: %d\n", error);
    }
}

// 演示 5: 重置请求结构
static void demo_reset(void) {
    printf("\n=== 演示 5: 重置请求结构 ===\n");

    http_req_t req;

    // 先解析一个请求
    const char *request = "GET /test HTTP/1.1\r\n";
    http_parse_request_line(request, strlen(request), &req);

    printf("解析后:\n");
    printf("  方法: %s\n", req.method);
    printf("  路径: %s\n", req.path);
    printf("  版本: %s\n", req.version);

    // 重置
    http_req_reset(&req);

    printf("\n重置后:\n");
    printf("  方法: \"%s\"\n", req.method);
    printf("  路径: \"%s\"\n", req.path);
    printf("  版本: \"%s\"\n", req.version);
}

// 演示 6: 性能优势
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能优势 ===\n");

    printf("极简解析器特点:\n\n");

    printf("1. 零内存分配\n");
    printf("   - 使用栈上缓冲区\n");
    printf("   - 无动态内存分配\n");
    printf("   - 适合嵌入式系统\n");
    printf("\n");

    printf("2. 极小代码体积\n");
    printf("   - 只解析请求行\n");
    printf("   - 无复杂状态机\n");
    printf("   - 快速编译\n");
    printf("\n");

    printf("3. 高性能\n");
    printf("   - O(n) 时间复杂度\n");
    printf("   - 单次遍历\n");
    printf("   - 无回溯\n");
    printf("\n");

    printf("4. 适用场景\n");
    printf("   - 资源受限环境\n");
    printf("   - 只需要请求行\n");
    printf("   - 快速路由决策\n");
}

// 演示 7: 使用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 7: 使用场景 ===\n");

    printf("1. 简单路由\n");
    printf("   - 根据方法和路径分发\n");
    printf("   - 快速拒绝无效请求\n");
    printf("\n");

    printf("2. 负载均衡\n");
    printf("   - 解析请求行获取路径\n");
    printf("   - 根据路径选择后端\n");
    printf("\n");

    printf("3. 日志记录\n");
    printf("   - 提取方法和路径\n");
    printf("   - 记录访问日志\n");
    printf("\n");

    printf("4. 限流控制\n");
    printf("   - 识别请求类型\n");
    printf("   - 应用不同限流策略\n");
}

int main(void) {
    printf("========================================\n");
    printf("    极简 HTTP 解析器演示\n");
    printf("========================================\n");

    demo_parse_request_line();
    demo_validate_method();
    demo_validate_version();
    demo_error_handling();
    demo_reset();
    demo_performance();
    demo_use_cases();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
