/**
 * @file demo_uuid.c
 * @brief UUID 生成和验证演示
 *
 * 本演示展示了 UUID 的基本操作，包括：
 * - UUID 生成
 * - UUID 解析
 * - UUID 验证
 * - UUID 比较
 * - UUID 版本获取
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/uuid.h"

// 演示 1: 生成 UUID v4
static void demo_generate_uuid(void) {
    printf("\n=== 演示 1: 生成 UUID v4 ===\n");

    // 使用简单的 uuid_v4 函数
    char uuid_str[40];
    uuid_v4(uuid_str);
    printf("生成的 UUID: %s\n", uuid_str);

    // 使用完整的上下文 API
    uuid_ctx_t *ctx = NULL;
    if (uuid_create(&ctx, NULL) == UUID_OK) {
        char uuid_str2[40];
        if (uuid_generate_v4_string(ctx, uuid_str2) == UUID_OK) {
            printf("使用上下文生成的 UUID: %s\n", uuid_str2);
        }
        uuid_destroy(ctx);
    }
}

// 演示 2: 解析 UUID
static void demo_parse_uuid(void) {
    printf("\n=== 演示 2: 解析 UUID ===\n");

    const char *uuid_str = "550e8400-e29b-41d4-a716-446655440000";
    printf("要解析的 UUID: %s\n", uuid_str);

    uuid_ctx_t *ctx = NULL;
    if (uuid_create(&ctx, NULL) == UUID_OK) {
        uuid_t uuid;
        if (uuid_parse(ctx, uuid_str, &uuid) == UUID_OK) {
            char out_str[40];
            if (uuid_to_string(ctx, &uuid, out_str) == UUID_OK) {
                printf("解析并重新生成: %s\n", out_str);
            }
        }
        uuid_destroy(ctx);
    }
}

// 演示 3: 验证 UUID
static void demo_validate_uuid(void) {
    printf("\n=== 演示 3: 验证 UUID ===\n");

    const char *test_uuids[] = {
        "550e8400-e29b-41d4-a716-446655440000", // 有效的 UUID
        "550e8400e29b41d4a716446655440000",     // 有效的 UUID (无连字符)
        "invalid-uuid",                         // 无效的 UUID
        "550e8400-e29b-41d4-a716-4466554400",   // 长度不足
        NULL
    };

    for (int i = 0; test_uuids[i]; i++) {
        const char *uuid_str = test_uuids[i];
        bool valid = uuid_is_valid(uuid_str);
        printf("UUID '%s' 是 %s\n", uuid_str, valid ? "有效的" : "无效的");
    }
}

// 演示 4: UUID 比较和版本获取
static void demo_uuid_operations(void) {
    printf("\n=== 演示 4: UUID 操作 ===\n");

    uuid_ctx_t *ctx = NULL;
    if (uuid_create(&ctx, NULL) == UUID_OK) {
        // 生成两个 UUID
        uuid_t uuid1, uuid2;
        if (uuid_generate_v4(ctx, &uuid1) == UUID_OK && uuid_generate_v4(ctx, &uuid2) == UUID_OK) {
            // 比较 UUID
            int cmp = uuid_compare(&uuid1, &uuid2);
            printf("UUID 比较结果: %d (0=相等, !=0=不相等)\n", cmp);

            // 获取 UUID 版本
            int version = uuid_get_version(&uuid1);
            printf("UUID 版本: %d\n", version);

            // 检查是否为 nil UUID
            bool is_nil = uuid_is_nil(&uuid1);
            printf("UUID 是否为 nil: %s\n", is_nil ? "是" : "否");

            // 复制 UUID
            uuid_t uuid3;
            uuid_copy(&uuid3, &uuid1);
            char out_str[40];
            if (uuid_to_string(ctx, &uuid3, out_str) == UUID_OK) {
                printf("复制的 UUID: %s\n", out_str);
            }
        }
        uuid_destroy(ctx);
    }
}

// 演示 5: UUID 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: UUID 应用场景 ===\n");

    printf("1. 唯一标识符:\n");
    printf("   - 数据库主键\n");
    printf("   - 分布式系统中的唯一标识\n");
    printf("   - 会话标识\n");
    printf("   - 消息队列消息 ID\n");

    printf("\n2. 安全相关:\n");
    printf("   - 令牌和会话 ID\n");
    printf("   - 密码重置令牌\n");
    printf("   - 确认邮件令牌\n");

    printf("\n3. 配置和元数据:\n");
    printf("   - 配置项标识\n");
    printf("   - 元数据记录标识\n");
    printf("   - 版本控制标识\n");

    printf("\n4. 分布式系统:\n");
    printf("   - 节点标识\n");
    printf("   - 事务标识\n");
    printf("   - 事件标识\n");
}

int main(void) {
    printf("========================================\n");
    printf("    UUID 生成和验证演示程序\n");
    printf("========================================\n");

    demo_generate_uuid();
    demo_parse_uuid();
    demo_validate_uuid();
    demo_uuid_operations();
    demo_applications();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}