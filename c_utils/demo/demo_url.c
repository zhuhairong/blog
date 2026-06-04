/**
 * URL 解析与构建演示程序
 *
 * 功能：
 * - URL 解析 (scheme, host, port, path, query, fragment)
 * - URL 构建
 * - URL 验证
 * - 查询参数操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/url.h"

// 演示 1: URL 解析
static void demo_url_parse(void) {
    printf("\n=== 演示 1: URL 解析 ===\n");

    url_ctx_t *ctx = NULL;
    url_error_t err = url_create(&ctx, NULL);
    if (err != URL_OK) {
        printf("创建上下文失败: %s\n", url_strerror(err));
        return;
    }

    const char *test_urls[] = {
        "https://www.example.com:8080/path/to/resource?key=value&lang=en#section1",
        "ftp://files.example.com/downloads/document.pdf",
        "http://localhost/api/v1/users?id=42",
        "https://user:pass@example.com/secure",
        NULL
    };

    for (int i = 0; test_urls[i]; i++) {
        printf("\nURL: %s\n", test_urls[i]);

        url_parse_result_t *result = NULL;
        err = url_parse(ctx, test_urls[i], &result);
        if (err == URL_OK && result) {
            printf("  Scheme:   %s\n", result->url.scheme ? result->url.scheme : "(none)");
            printf("  Host:     %s\n", result->url.host ? result->url.host : "(none)");
            printf("  Port:     %d\n", result->url.port);
            printf("  Path:     %s\n", result->url.path ? result->url.path : "(none)");
            printf("  Query:    %s\n", result->url.query ? result->url.query : "(none)");
            printf("  Fragment: %s\n", result->url.fragment ? result->url.fragment : "(none)");

            if (result->param_count > 0) {
                printf("  查询参数 (%zu):\n", result->param_count);
                for (size_t j = 0; j < result->param_count; j++) {
                    printf("    %s = %s\n", result->params[j].key, result->params[j].value);
                }
            }

            url_parse_result_destroy(result);
        } else {
            printf("  解析失败: %s\n", url_strerror(err));
        }
    }

    url_destroy(ctx);
}

// 演示 2: URL 构建
static void demo_url_build(void) {
    printf("\n=== 演示 2: URL 构建 ===\n");

    url_ctx_t *ctx = NULL;
    url_create(&ctx, NULL);

    url_t url1 = {
        .scheme = "https",
        .host = "api.example.com",
        .port = 0,  // 使用默认端口
        .path = "/v2/data",
        .query = "format=json&limit=100",
        .fragment = NULL
    };

    size_t out_size = 0;
    url_error_t err = url_build(ctx, &url1, NULL, &out_size);
    if (err == URL_OK) {
        char *built = malloc(out_size);
        url_build(ctx, &url1, built, &out_size);
        printf("构建的 URL: %s\n", built);
        free(built);
    }

    url_t url2 = {
        .scheme = "http",
        .host = "localhost",
        .port = 3000,
        .path = "/admin",
        .query = NULL,
        .fragment = "top"
    };

    err = url_build(ctx, &url2, NULL, &out_size);
    if (err == URL_OK) {
        char *built = malloc(out_size);
        url_build(ctx, &url2, built, &out_size);
        printf("构建的 URL: %s\n", built);
        free(built);
    }

    url_destroy(ctx);
}

// 演示 3: URL 验证
static void demo_url_validate(void) {
    printf("\n=== 演示 3: URL 验证 ===\n");

    url_ctx_t *ctx = NULL;
    url_create(&ctx, NULL);

    const char *test_urls[] = {
        "https://www.google.com",
        "http://example.com/path?query=value",
        "not_a_valid_url",
        "ftp://files.example.com",
        "://missing-scheme.com",
        NULL
    };

    for (int i = 0; test_urls[i]; i++) {
        url_error_t err = url_validate(ctx, test_urls[i]);
        printf("  %s", test_urls[i]);
        if (err == URL_OK) {
            printf(" ✓ 有效\n");
        } else {
            printf(" ✗ 无效 (%s)\n", url_strerror(err));
        }
    }

    url_destroy(ctx);
}

// 演示 4: 查询参数操作
static void demo_query_params(void) {
    printf("\n=== 演示 4: 查询参数操作 ===\n");

    url_ctx_t *ctx = NULL;
    url_create(&ctx, NULL);

    const char *base_url = "https://search.example.com/results?q=test&page=1";
    printf("原始 URL: %s\n\n", base_url);

    url_parse_result_t *result = NULL;
    url_error_t err = url_parse(ctx, base_url, &result);
    if (err != URL_OK) {
        printf("解析失败\n");
        url_destroy(ctx);
        return;
    }

    // 获取查询参数
    printf("获取查询参数:\n");
    char *value = NULL;
    err = url_get_query_param(result, "q", &value);
    if (err == URL_OK) {
        printf("  q = %s\n", value);
    }

    err = url_get_query_param(result, "page", &value);
    if (err == URL_OK) {
        printf("  page = %s\n", value);
    }

    // 添加查询参数
    printf("\n添加查询参数:\n");
    err = url_add_query_param(result, "sort", "date");
    printf("  添加 sort=date: %s\n", err == URL_OK ? "成功" : url_strerror(err));

    err = url_add_query_param(result, "lang", "en");
    printf("  添加 lang=en: %s\n", err == URL_OK ? "成功" : url_strerror(err));

    // 移除查询参数
    printf("\n移除查询参数:\n");
    err = url_remove_query_param(result, "page");
    printf("  移除 page: %s\n", err == URL_OK ? "成功" : url_strerror(err));

    // 重新构建 URL
    size_t out_size = 0;
    err = url_build(ctx, &result->url, NULL, &out_size);
    if (err == URL_OK) {
        char *built = malloc(out_size);
        url_build(ctx, &result->url, built, &out_size);
        printf("\n最终 URL: %s\n", built);
        free(built);
    }

    url_parse_result_destroy(result);
    url_destroy(ctx);
}

// 演示 5: 编码/解码（通过 url.h 的 encode/decode 接口）
static void demo_encode_decode(void) {
    printf("\n=== 演示 5: URL 编码/解码 ===\n");

    url_ctx_t *ctx = NULL;
    url_create(&ctx, NULL);

    const char *raw = "hello world! foo=bar&x=y";
    printf("原始字符串: %s\n", raw);

    // 编码
    size_t enc_size = 0;
    url_error_t err = url_encode_ex(ctx, raw, NULL, &enc_size);
    if (err == URL_OK) {
        char *encoded = malloc(enc_size);
        url_encode_ex(ctx, raw, encoded, &enc_size);
        printf("编码后:     %s\n", encoded);

        // 解码
        size_t dec_size = 0;
        err = url_decode_ex(ctx, encoded, NULL, &dec_size);
        if (err == URL_OK) {
            char *decoded = malloc(dec_size);
            url_decode_ex(ctx, encoded, decoded, &dec_size);
            printf("解码后:     %s\n", decoded);
            free(decoded);
        }
        free(encoded);
    }

    url_destroy(ctx);
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. Web 浏览器\n");
    printf("   - URL 解析和规范化\n");
    printf("   - 相对 URL 解析\n");
    printf("   - 查询参数提取\n\n");

    printf("2. HTTP 客户端\n");
    printf("   - 构建请求 URL\n");
    printf("   - 参数编码\n");
    printf("   - URL 验证\n\n");

    printf("3. REST API\n");
    printf("   - 动态添加/移除查询参数\n");
    printf("   - URL 路由解析\n");
    printf("   - 分页参数处理\n\n");

    printf("4. 网络爬虫\n");
    printf("   - URL 规范化\n");
    printf("   - 提取域名/路径\n");
    printf("   - 过滤特定模式\n\n");

    printf("5. 日志分析\n");
    printf("   - 解析访问日志中的 URL\n");
    printf("   - 提取路径和参数\n");
    printf("   - 统计分析\n");
}

int main(void) {
    printf("========================================\n");
    printf("    URL 解析与构建演示\n");
    printf("========================================\n");

    demo_url_parse();
    demo_url_build();
    demo_url_validate();
    demo_query_params();
    demo_encode_decode();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
