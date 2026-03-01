#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

void demo_version_parse(void) {
    printf("\n=== 版本解析演示 ===\n");
    
    version_ctx_t* ctx = NULL;
    version_config_t config = {
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .strict_mode = false,
        .max_version_length = 256
    };
    
    version_error_t err = version_create(&ctx, &config);
    if (err != VERSION_OK) {
        printf("创建上下文失败: %s\n", version_strerror(err));
        return;
    }
    
    const char* versions[] = {
        "1.2.3",
        "2.0.0-alpha.1",
        "3.1.4-beta+build.123",
        "1.0.0-rc.1"
    };
    
    for (int i = 0; i < 4; i++) {
        version_t v;
        err = version_parse(ctx, versions[i], &v);
        if (err == VERSION_OK) {
            char out[256] = {0};
            version_to_string(ctx, &v, out, sizeof(out));
            printf("解析 '%s' -> %s\n", versions[i], out);
            version_free(&v);
        }
    }
    
    version_destroy(ctx);
}

void demo_version_compare(void) {
    printf("\n=== 版本比较演示 ===\n");
    
    version_ctx_t* ctx = NULL;
    version_error_t err = version_create(&ctx, NULL);
    if (err != VERSION_OK) {
        printf("创建上下文失败: %s\n", version_strerror(err));
        return;
    }
    
    version_t v1, v2;
    version_parse(ctx, "1.2.3", &v1);
    version_parse(ctx, "1.2.4", &v2);
    
    int result;
    err = version_compare_safe(ctx, v1, v2, &result);
    if (err == VERSION_OK) {
        printf("1.2.3 vs 1.2.4: ");
        if (result < 0) printf("1.2.3 < 1.2.4\n");
        else if (result > 0) printf("1.2.3 > 1.2.4\n");
        else printf("1.2.3 == 1.2.4\n");
    }
    
    version_parse(ctx, "2.0.0-alpha", &v1);
    version_parse(ctx, "2.0.0", &v2);
    
    err = version_compare_safe(ctx, v1, v2, &result);
    if (err == VERSION_OK) {
        printf("2.0.0-alpha vs 2.0.0: ");
        if (result < 0) printf("2.0.0-alpha < 2.0.0\n");
        else if (result > 0) printf("2.0.0-alpha > 2.0.0\n");
        else printf("2.0.0-alpha == 2.0.0\n");
    }
    
    version_free(&v1);
    version_free(&v2);
    version_destroy(ctx);
}

void demo_version_increment(void) {
    printf("\n=== 版本增量演示 ===\n");
    
    version_ctx_t* ctx = NULL;
    version_error_t err = version_create(&ctx, NULL);
    if (err != VERSION_OK) {
        printf("创建上下文失败: %s\n", version_strerror(err));
        return;
    }
    
    version_t v;
    version_parse(ctx, "1.2.3", &v);
    
    char out[256] = {0};
    version_to_string(ctx, &v, out, sizeof(out));
    printf("原始版本: %s\n", out);
    
    version_increment(ctx, &v, 0);
    version_to_string(ctx, &v, out, sizeof(out));
    printf("增加主版本号: %s\n", out);
    
    version_increment(ctx, &v, 1);
    version_to_string(ctx, &v, out, sizeof(out));
    printf("增加次版本号: %s\n", out);
    
    version_increment(ctx, &v, 2);
    version_to_string(ctx, &v, out, sizeof(out));
    printf("增加修订号: %s\n", out);
    
    version_free(&v);
    version_destroy(ctx);
}

void demo_version_validation(void) {
    printf("\n=== 版本验证演示 ===\n");
    
    version_ctx_t* ctx = NULL;
    version_error_t err = version_create(&ctx, NULL);
    if (err != VERSION_OK) {
        printf("创建上下文失败: %s\n", version_strerror(err));
        return;
    }
    
    const char* test_versions[] = {
        "1.2.3",
        "2.0.0-alpha",
        "invalid",
        "1.2.3.4",
        "1.2"
    };
    
    for (int i = 0; i < 5; i++) {
        version_t v;
        err = version_parse(ctx, test_versions[i], &v);
        if (err == VERSION_OK) {
            bool valid = version_is_valid(&v);
            printf("'%s': %s\n", test_versions[i], valid ? "有效" : "无效");
            version_free(&v);
        } else {
            printf("'%s': 解析失败\n", test_versions[i]);
        }
    }
    
    version_destroy(ctx);
}

void demo_version_copy(void) {
    printf("\n=== 版本复制演示 ===\n");
    
    version_ctx_t* ctx = NULL;
    version_error_t err = version_create(&ctx, NULL);
    if (err != VERSION_OK) {
        printf("创建上下文失败: %s\n", version_strerror(err));
        return;
    }
    
    version_t v1, v2;
    version_parse(ctx, "1.2.3-alpha+build.123", &v1);
    
    err = version_copy(&v2, &v1);
    if (err == VERSION_OK) {
        char out1[256] = {0}, out2[256] = {0};
        version_to_string(ctx, &v1, out1, sizeof(out1));
        version_to_string(ctx, &v2, out2, sizeof(out2));
        printf("原始版本: %s\n", out1);
        printf("复制版本: %s\n", out2);
    }
    
    version_free(&v1);
    version_free(&v2);
    version_destroy(ctx);
}

int main(void) {
    printf("========================================\n");
    printf("    版本管理演示程序\n");
    printf("========================================\n");
    
    demo_version_parse();
    demo_version_compare();
    demo_version_increment();
    demo_version_validation();
    demo_version_copy();
    
    printf("\n=== 版本管理演示完成 ===\n");
    return 0;
}
