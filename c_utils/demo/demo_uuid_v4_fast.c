#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uuid_v4_fast.h"

void demo_basic_generation(void) {
    printf("\n=== 基本UUID v4生成演示 ===\n");
    
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .use_entropy_pool = false,
        .prng_seed_size = 0
    };
    
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, &config);
    if (err != UUID_V4_FAST_OK) {
        printf("创建上下文失败: %s\n", uuid_v4_fast_strerror(err));
        return;
    }
    
    char uuid_str[37] = {0};
    for (int i = 0; i < 5; i++) {
        err = uuid_v4_fast_generate_string(ctx, uuid_str);
        if (err == UUID_V4_FAST_OK) {
            printf("UUID %d: %s\n", i + 1, uuid_str);
        }
    }
    
    uuid_v4_fast_destroy(ctx);
}

void demo_binary_generation(void) {
    printf("\n=== 二进制格式UUID生成演示 ===\n");
    
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, NULL);
    if (err != UUID_V4_FAST_OK) {
        printf("创建上下文失败: %s\n", uuid_v4_fast_strerror(err));
        return;
    }
    
    uint8_t uuid[16] = {0};
    err = uuid_v4_fast_generate(ctx, uuid);
    if (err == UUID_V4_FAST_OK) {
        printf("二进制UUID: ");
        for (int i = 0; i < 16; i++) {
            printf("%02x", uuid[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) printf("-");
        }
        printf("\n");
    }
    
    uuid_v4_fast_destroy(ctx);
}

void demo_batch_generation(void) {
    printf("\n=== 批量生成UUID演示 ===\n");
    
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, NULL);
    if (err != UUID_V4_FAST_OK) {
        printf("创建上下文失败: %s\n", uuid_v4_fast_strerror(err));
        return;
    }
    
    uint8_t uuids[5][16] = {0};
    err = uuid_v4_fast_generate_batch(ctx, (uint8_t*)uuids, 5);
    if (err == UUID_V4_FAST_OK) {
        printf("批量生成了5个UUID:\n");
        for (int i = 0; i < 5; i++) {
            printf("  UUID %d: ", i + 1);
            for (int j = 0; j < 16; j++) {
                printf("%02x", uuids[i][j]);
            }
            printf("\n");
        }
    }
    
    uint64_t count = uuid_v4_fast_get_generate_count(ctx);
    printf("总共生成了 %llu 个UUID\n", (unsigned long long)count);
    
    uuid_v4_fast_destroy(ctx);
}

void demo_uppercase_uuid(void) {
    printf("\n=== 大写UUID演示 ===\n");
    
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_config_t config = {
        .use_hyphens = true,
        .uppercase = true,
        .use_entropy_pool = false,
        .prng_seed_size = 0
    };
    
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, &config);
    if (err != UUID_V4_FAST_OK) {
        printf("创建上下文失败: %s\n", uuid_v4_fast_strerror(err));
        return;
    }
    
    char uuid_str[37] = {0};
    err = uuid_v4_fast_generate_string(ctx, uuid_str);
    if (err == UUID_V4_FAST_OK) {
        printf("大写UUID: %s\n", uuid_str);
    }
    
    uuid_v4_fast_destroy(ctx);
}

void demo_no_hyphens(void) {
    printf("\n=== 无连字符UUID演示 ===\n");
    
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_config_t config = {
        .use_hyphens = false,
        .uppercase = false,
        .use_entropy_pool = false,
        .prng_seed_size = 0
    };
    
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, &config);
    if (err != UUID_V4_FAST_OK) {
        printf("创建上下文失败: %s\n", uuid_v4_fast_strerror(err));
        return;
    }
    
    char uuid_str[37] = {0};
    err = uuid_v4_fast_generate_string(ctx, uuid_str);
    if (err == UUID_V4_FAST_OK) {
        printf("无连字符UUID: %s\n", uuid_str);
    }
    
    uuid_v4_fast_destroy(ctx);
}

int main(void) {
    printf("========================================\n");
    printf("    UUID v4 快速生成演示程序\n");
    printf("========================================\n");
    
    demo_basic_generation();
    demo_binary_generation();
    demo_batch_generation();
    demo_uppercase_uuid();
    demo_no_hyphens();
    
    printf("\n=== UUID v4 快速生成演示完成 ===\n");
    return 0;
}
