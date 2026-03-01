/**
 * 版本比较演示程序
 *
 * 功能：
 * - 语义版本比较
 * - 版本范围检查
 * - 版本验证
 * - 版本排序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/version_compare.h"

// 演示 1: 基本版本比较
static void demo_basic_compare(void) {
    printf("\n=== 演示 1: 基本版本比较 ===\n");

    // 创建版本比较上下文
    version_compare_ctx_t *ctx = NULL;
    version_compare_config_t config = {
        .strict_mode = true,
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .use_semver_rules = true,
        .max_version_length = 128
    };

    version_compare_error_t error = version_compare_create(&ctx, &config);
    if (error != VERSION_COMPARE_OK) {
        printf("创建版本比较上下文失败: %s\n", version_compare_strerror(error));
        return;
    }

    // 测试不同版本组合
    struct {
        const char *v1;
        const char *v2;
        const char *description;
    } test_cases[] = {
        {"1.0.0", "1.0.0", "相同版本"},
        {"1.0.0", "1.0.1", "补丁版本不同"},
        {"1.0.0", "1.1.0", "次版本不同"},
        {"1.0.0", "2.0.0", "主版本不同"},
        {"1.0.0-alpha", "1.0.0", "预发布版本"},
        {"1.0.0-alpha.1", "1.0.0-alpha.2", "预发布版本序号"},
        {"1.0.0+build.1", "1.0.0+build.2", "构建元数据"},
    };

    size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (size_t i = 0; i < test_count; i++) {
        const char *v1 = test_cases[i].v1;
        const char *v2 = test_cases[i].v2;
        const char *desc = test_cases[i].description;

        int result = 0;
        error = version_compare_str_safe(ctx, v1, v2, &result);
        if (error == VERSION_COMPARE_OK) {
            printf("%s: %s vs %s -> ", desc, v1, v2);
            if (result < 0) {
                printf("%s < %s\n", v1, v2);
            } else if (result > 0) {
                printf("%s > %s\n", v1, v2);
            } else {
                printf("%s == %s\n", v1, v2);
            }
        } else {
            printf("比较 %s vs %s 失败: %s\n", v1, v2, version_compare_strerror(error));
        }
    }

    version_compare_destroy(ctx);
}

// 演示 2: 版本范围检查
static void demo_range_check(void) {
    printf("\n=== 演示 2: 版本范围检查 ===\n");

    // 创建版本比较上下文
    version_compare_ctx_t *ctx = NULL;
    version_compare_config_t config = {
        .strict_mode = true,
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .use_semver_rules = true,
        .max_version_length = 128
    };

    version_compare_error_t error = version_compare_create(&ctx, &config);
    if (error != VERSION_COMPARE_OK) {
        printf("创建版本比较上下文失败: %s\n", version_compare_strerror(error));
        return;
    }

    // 测试版本范围
    struct {
        const char *version;
        const char *range;
        const char *description;
    } test_cases[] = {
        {"1.2.3", "^1.0.0", "兼容版本"},
        {"1.2.3", "~1.2.0", "补丁版本范围"},
        {"2.0.0", "^1.0.0", "主版本不兼容"},
        {"1.1.0", "^1.2.0", "次版本不兼容"},
        {"1.2.4", "~1.2.3", "补丁版本超出"},
    };

    size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (size_t i = 0; i < test_count; i++) {
        const char *version = test_cases[i].version;
        const char *range = test_cases[i].range;
        const char *desc = test_cases[i].description;

        bool in_range = version_compare_in_range(ctx, version, range);
        printf("%s: %s in %s -> %s\n", desc, version, range, in_range ? "是" : "否");
    }

    version_compare_destroy(ctx);
}

// 演示 3: 版本验证
static void demo_validation(void) {
    printf("\n=== 演示 3: 版本验证 ===\n");

    // 创建版本比较上下文
    version_compare_ctx_t *ctx = NULL;
    version_compare_config_t config = {
        .strict_mode = true,
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .use_semver_rules = true,
        .max_version_length = 128
    };

    version_compare_error_t error = version_compare_create(&ctx, &config);
    if (error != VERSION_COMPARE_OK) {
        printf("创建版本比较上下文失败: %s\n", version_compare_strerror(error));
        return;
    }

    // 测试版本格式
    const char *versions[] = {
        "1.0.0",          // 有效
        "1.0.0-alpha",    // 有效（预发布）
        "1.0.0+build.1",  // 有效（构建元数据）
        "1.0",            // 无效（缺少补丁版本）
        "1",              // 无效（缺少次版本和补丁版本）
        "1.0.0.0",        // 无效（多余部分）
        "1.0.0-alpha.1+build.2",  // 有效（完整格式）
    };

    size_t versions_count = sizeof(versions) / sizeof(versions[0]);

    for (size_t i = 0; i < versions_count; i++) {
        const char *version = versions[i];
        bool valid = version_compare_validate(ctx, version);
        printf("版本 %s -> %s\n", version, valid ? "有效" : "无效");
    }

    version_compare_destroy(ctx);
}

// 演示 4: 版本排序
static void demo_sort(void) {
    printf("\n=== 演示 4: 版本排序 ===\n");

    // 创建版本比较上下文
    version_compare_ctx_t *ctx = NULL;
    version_compare_config_t config = {
        .strict_mode = true,
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .use_semver_rules = true,
        .max_version_length = 128
    };

    version_compare_error_t error = version_compare_create(&ctx, &config);
    if (error != VERSION_COMPARE_OK) {
        printf("创建版本比较上下文失败: %s\n", version_compare_strerror(error));
        return;
    }

    // 准备版本数组
    char *versions[] = {
        "1.0.0",
        "1.1.0",
        "1.0.1",
        "2.0.0",
        "1.0.0-alpha",
        "1.0.0-beta",
    };

    size_t versions_count = sizeof(versions) / sizeof(versions[0]);

    // 显示原始顺序
    printf("原始顺序:\n");
    for (size_t i = 0; i < versions_count; i++) {
        printf("  %s\n", versions[i]);
    }

    // 升序排序
    error = version_compare_sort(ctx, versions, versions_count, true);
    if (error == VERSION_COMPARE_OK) {
        printf("\n升序排序:\n");
        for (size_t i = 0; i < versions_count; i++) {
            printf("  %s\n", versions[i]);
        }
    } else {
        printf("\n排序失败: %s\n", version_compare_strerror(error));
    }

    // 降序排序
    error = version_compare_sort(ctx, versions, versions_count, false);
    if (error == VERSION_COMPARE_OK) {
        printf("\n降序排序:\n");
        for (size_t i = 0; i < versions_count; i++) {
            printf("  %s\n", versions[i]);
        }
    } else {
        printf("\n排序失败: %s\n", version_compare_strerror(error));
    }

    version_compare_destroy(ctx);
}

// 演示 5: 批量版本比较
static void demo_batch_compare(void) {
    printf("\n=== 演示 5: 批量版本比较 ===\n");

    // 创建版本比较上下文
    version_compare_ctx_t *ctx = NULL;
    version_compare_config_t config = {
        .strict_mode = true,
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .use_semver_rules = true,
        .max_version_length = 128
    };

    version_compare_error_t error = version_compare_create(&ctx, &config);
    if (error != VERSION_COMPARE_OK) {
        printf("创建版本比较上下文失败: %s\n", version_compare_strerror(error));
        return;
    }

    // 基准版本
    const char *base_version = "1.0.0";

    // 要比较的版本数组
    const char *versions[] = {
        "0.9.0",
        "1.0.0",
        "1.0.1",
        "1.1.0",
        "2.0.0",
    };

    size_t versions_count = sizeof(versions) / sizeof(versions[0]);
    int results[versions_count];

    // 批量比较
    error = version_compare_batch(ctx, versions, versions_count, results);
    if (error == VERSION_COMPARE_OK) {
        printf("与 %s 比较:\n", base_version);
        for (size_t i = 0; i < versions_count; i++) {
            const char *v = versions[i];
            int result = results[i];
            printf("  %s vs %s -> ", v, base_version);
            if (result < 0) {
                printf("%s < %s\n", v, base_version);
            } else if (result > 0) {
                printf("%s > %s\n", v, base_version);
            } else {
                printf("%s == %s\n", v, base_version);
            }
        }
    } else {
        printf("批量比较失败: %s\n", version_compare_strerror(error));
    }

    version_compare_destroy(ctx);
}

// 演示 6: 传统API使用
static void demo_legacy_api(void) {
    printf("\n=== 演示 6: 传统API使用 ===\n");

    // 使用传统API比较版本
    struct {
        const char *v1;
        const char *v2;
    } test_cases[] = {
        {"1.0.0", "1.0.1"},
        {"1.1.0", "1.0.0"},
        {"2.0.0", "1.9.9"},
    };

    size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    for (size_t i = 0; i < test_count; i++) {
        const char *v1 = test_cases[i].v1;
        const char *v2 = test_cases[i].v2;
        int result = version_compare_str(v1, v2);

        printf("%s vs %s -> ", v1, v2);
        if (result < 0) {
            printf("%s < %s\n", v1, v2);
        } else if (result > 0) {
            printf("%s > %s\n", v1, v2);
        } else {
            printf("%s == %s\n", v1, v2);
        }
    }
}

int main(void) {
    printf("========================================\n");
    printf("    版本比较演示\n");
    printf("========================================\n");

    demo_basic_compare();
    demo_range_check();
    demo_validation();
    demo_sort();
    demo_batch_compare();
    demo_legacy_api();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
