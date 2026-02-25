/**
 * HTTP 解析演示程序
 *
 * 功能：
 * - HTTP 请求解析
 * - HTTP 响应解析
 * - 头部处理
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/http.h"

// 演示 1: 解析 HTTP 请求
static void demo_parse_request(void) {
    printf("\n=== 演示 1: HTTP 请求解析 ===\n");

    http_error_t error;

    const char *request =
        "GET /api/users?id=123 HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Accept: application/json\r\n"
        "Authorization: Bearer token123\r\n"
        "\r\n";

    printf("原始 HTTP 请求:\n%s\n", request);

    http_req_t *req = http_parse_request(request, NULL, &error);
    if (req) {
        printf("解析结果:\n");
        printf("  方法: %s\n", req->method ? req->method : "N/A");
        printf("  路径: %s\n", req->path ? req->path : "N/A");
        printf("  查询: %s\n", req->query ? req->query : "N/A");
        printf("  版本: %s\n", req->version ? req->version : "N/A");
        printf("  头部数量: %zu\n", req->header_count);

        printf("\n头部信息:\n");
        for (size_t i = 0; i < req->header_count; i++) {
            printf("  %s: %s\n", req->headers[i], req->values[i]);
        }

        http_req_free(req);
    } else {
        printf("解析失败: %d\n", error);
    }
}

// 演示 2: 解析 HTTP 响应
static void demo_parse_response(void) {
    printf("\n=== 演示 2: HTTP 响应解析 ===\n");

    http_error_t error;

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 52\r\n"
        "Server: nginx/1.18.0\r\n"
        "Cache-Control: no-cache\r\n"
        "\r\n"
        "{\"status\":\"success\",\"data\":{\"id\":123,\"name\":\"John\"}}";

    printf("原始 HTTP 响应:\n%s\n", response);

    http_res_t *res = http_parse_response(response, NULL, &error);
    if (res) {
        printf("解析结果:\n");
        printf("  版本: %s\n", res->version ? res->version : "N/A");
        printf("  状态码: %d\n", res->status_code);
        printf("  状态消息: %s\n", res->status_message ? res->status_message : "N/A");
        printf("  头部数量: %zu\n", res->header_count);

        printf("\n头部信息:\n");
        for (size_t i = 0; i < res->header_count; i++) {
            printf("  %s: %s\n", res->headers[i], res->values[i]);
        }

        if (res->body && res->body_len > 0) {
            printf("\n响应体 (%zu 字节):\n  %s\n", res->body_len, res->body);
        }

        http_res_free(res);
    } else {
        printf("解析失败: %d\n", error);
    }
}

// 演示 3: 不同 HTTP 方法
static void demo_http_methods(void) {
    printf("\n=== 演示 3: HTTP 方法 ===\n");

    http_error_t error;

    const char *methods[] = {"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"};

    for (int i = 0; i < 7; i++) {
        char request[256];
        snprintf(request, sizeof(request),
                 "%s /resource HTTP/1.1\r\nHost: example.com\r\n\r\n",
                 methods[i]);

        http_req_t *req = http_parse_request(request, NULL, &error);
        if (req) {
            printf("  %s - %s\n", methods[i],
                   strcmp(methods[i], req->method) == 0 ? "✓" : "✗");
            http_req_free(req);
        }
    }
}

// 演示 4: 状态码说明
static void demo_status_codes(void) {
    printf("\n=== 演示 4: HTTP 状态码 ===\n");

    printf("1xx - 信息性状态码:\n");
    printf("  100 Continue\n");
    printf("  101 Switching Protocols\n");
    printf("\n");

    printf("2xx - 成功:\n");
    printf("  200 OK\n");
    printf("  201 Created\n");
    printf("  204 No Content\n");
    printf("\n");

    printf("3xx - 重定向:\n");
    printf("  301 Moved Permanently\n");
    printf("  302 Found\n");
    printf("  304 Not Modified\n");
    printf("\n");

    printf("4xx - 客户端错误:\n");
    printf("  400 Bad Request\n");
    printf("  401 Unauthorized\n");
    printf("  403 Forbidden\n");
    printf("  404 Not Found\n");
    printf("\n");

    printf("5xx - 服务器错误:\n");
    printf("  500 Internal Server Error\n");
    printf("  502 Bad Gateway\n");
    printf("  503 Service Unavailable\n");
}

// 演示 5: 常见头部字段
static void demo_headers(void) {
    printf("\n=== 演示 5: 常见 HTTP 头部 ===\n");

    printf("通用头部:\n");
    printf("  Cache-Control    - 缓存控制\n");
    printf("  Connection       - 连接管理\n");
    printf("  Date             - 消息日期\n");
    printf("\n");

    printf("请求头部:\n");
    printf("  Accept           - 可接受的媒体类型\n");
    printf("  Authorization    - 认证信息\n");
    printf("  Host             - 目标主机\n");
    printf("  User-Agent       - 客户端信息\n");
    printf("\n");

    printf("响应头部:\n");
    printf("  Content-Type     - 内容类型\n");
    printf("  Content-Length   - 内容长度\n");
    printf("  Server           - 服务器信息\n");
    printf("  Set-Cookie       - 设置 Cookie\n");
    printf("\n");

    printf("实体头部:\n");
    printf("  Content-Encoding - 内容编码\n");
    printf("  Last-Modified    - 最后修改时间\n");
    printf("  Expires          - 过期时间\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. HTTP 客户端\n");
    printf("   - 解析服务器响应\n");
    printf("   - 提取状态和数据\n");
    printf("\n");

    printf("2. HTTP 服务器\n");
    printf("   - 解析客户端请求\n");
    printf("   - 路由分发\n");
    printf("\n");

    printf("3. 代理服务器\n");
    printf("   - 请求/响应转发\n");
    printf("   - 头部修改\n");
    printf("\n");

    printf("4. API 网关\n");
    printf("   - 请求验证\n");
    printf("   - 鉴权处理\n");
    printf("\n");

    printf("5. 日志分析\n");
    printf("   - 解析 HTTP 日志\n");
    printf("   - 统计分析\n");
}

int main(void) {
    printf("========================================\n");
    printf("    HTTP 解析演示\n");
    printf("========================================\n");

    demo_parse_request();
    demo_parse_response();
    demo_http_methods();
    demo_status_codes();
    demo_headers();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
