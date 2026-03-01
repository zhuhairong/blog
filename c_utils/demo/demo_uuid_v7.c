#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uuid_v7.h"

void demo_basic_generation(void) {
    printf("\n=== 基本UUID v7生成演示 ===\n");
    
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .use_monotonic = true,
        .use_entropy = true,
        .entropy_size = 0
    };
    
    uuid_v7_error_t err = uuid_v7_create(&ctx, &config);
    if (err != UUID_V7_OK) {
        printf("创建上下文失败: %s\n", uuid_v7_strerror(err));
        return;
    }
    
    char uuid_str[37] = {0};
    for (int i = 0; i < 5; i++) {
        err = uuid_v7_generate_string(ctx, uuid_str);
        if (err == UUID_V7_OK) {
            printf("UUID v7 %d: %s\n", i + 1, uuid_str);
        }
    }
    
    uuid_v7_destroy(ctx);
}

void demo_timestamp_extraction(void) {
    printf("\n=== 时间戳提取演示 ===\n");
    
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_error_t err = uuid_v7_create(&ctx, NULL);
    if (err != UUID_V7_OK) {
        printf("创建上下文失败: %s\n", uuid_v7_strerror(err));
        return;
    }
    
    uint8_t uuid[16] = {0};
    err = uuid_v7_generate(ctx, uuid);
    if (err == UUID_V7_OK) {
        uint64_t timestamp = 0;
        err = uuid_v7_extract_timestamp(uuid, &timestamp);
        if (err == UUID_V7_OK) {
            printf("UUID v7 中的时间戳: %llu 毫秒\n", (unsigned long long)timestamp);
            
            time_t seconds = timestamp / 1000;
            struct tm* tm_info = gmtime(&seconds);
            char time_str[64] = {0};
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("对应的时间: %s UTC\n", time_str);
        }
    }
    
    uuid_v7_destroy(ctx);
}

void demo_custom_timestamp(void) {
    printf("\n=== 自定义时间戳生成UUID演示 ===\n");
    
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_error_t err = uuid_v7_create(&ctx, NULL);
    if (err != UUID_V7_OK) {
        printf("创建上下文失败: %s\n", uuid_v7_strerror(err));
        return;
    }
    
    uint64_t custom_timestamp = 1700000000000ULL;
    printf("使用自定义时间戳: %llu 毫秒\n", (unsigned long long)custom_timestamp);
    
    uint8_t uuid[16] = {0};
    err = uuid_v7_generate_with_timestamp(ctx, custom_timestamp, uuid);
    if (err == UUID_V7_OK) {
        printf("生成的UUID v7: ");
        for (int i = 0; i < 16; i++) {
            printf("%02x", uuid[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) printf("-");
        }
        printf("\n");
        
        uint64_t extracted_ts = 0;
        uuid_v7_extract_timestamp(uuid, &extracted_ts);
        printf("提取的时间戳: %llu 毫秒\n", (unsigned long long)extracted_ts);
    }
    
    uuid_v7_destroy(ctx);
}

void demo_batch_generation(void) {
    printf("\n=== 批量生成UUID v7演示 ===\n");
    
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_error_t err = uuid_v7_create(&ctx, NULL);
    if (err != UUID_V7_OK) {
        printf("创建上下文失败: %s\n", uuid_v7_strerror(err));
        return;
    }
    
    uint8_t uuids[5][16] = {0};
    err = uuid_v7_generate_batch(ctx, (uint8_t*)uuids, 5);
    if (err == UUID_V7_OK) {
        printf("批量生成了5个UUID v7:\n");
        for (int i = 0; i < 5; i++) {
            printf("  UUID %d: ", i + 1);
            for (int j = 0; j < 16; j++) {
                printf("%02x", uuids[i][j]);
            }
            printf("\n");
        }
    }
    
    uuid_v7_destroy(ctx);
}

void demo_monotonic_ordering(void) {
    printf("\n=== 单调递增特性演示 ===\n");
    
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_config_t config = {
        .use_hyphens = false,
        .uppercase = false,
        .use_monotonic = true,
        .use_entropy = true,
        .entropy_size = 0
    };
    
    uuid_v7_error_t err = uuid_v7_create(&ctx, &config);
    if (err != UUID_V7_OK) {
        printf("创建上下文失败: %s\n", uuid_v7_strerror(err));
        return;
    }
    
    printf("生成10个UUID v7，观察排序特性:\n");
    char uuid_str[33] = {0};
    for (int i = 0; i < 10; i++) {
        err = uuid_v7_generate_string(ctx, uuid_str);
        if (err == UUID_V7_OK) {
            printf("  %s\n", uuid_str);
        }
    }
    
    uuid_v7_destroy(ctx);
}

int main(void) {
    printf("========================================\n");
    printf("    UUID v7 演示程序\n");
    printf("========================================\n");
    
    demo_basic_generation();
    demo_timestamp_extraction();
    demo_custom_timestamp();
    demo_batch_generation();
    demo_monotonic_ordering();
    
    printf("\n=== UUID v7 演示完成 ===\n");
    return 0;
}
