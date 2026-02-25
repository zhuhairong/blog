/**
 * 随机数生成器演示程序
 *
 * 功能：
 * - 多种随机数算法
 * - 范围生成
 * - 应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/random.h"

static void demo_basic(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    printf("设置种子 (seed = 12345):\n");
    random_seed(12345);

    printf("\n生成随机整数:\n");
    printf("  random_u32():  %u\n", random_u32());
    printf("  random_u32():  %u\n", random_u32());
    printf("  random_u64():  %lu\n", random_u64());
    printf("  random_i32():  %d\n", random_i32());
    printf("  random_i64():  %ld\n", random_i64());

    printf("\n使用系统熵设置种子:\n");
    if (random_seed_with_entropy()) {
        printf("  成功! 生成随机数:\n");
        printf("  random_u32():  %u\n", random_u32());
    } else {
        printf("  失败\n");
    }
}

static void demo_range(void) {
    printf("\n=== 演示 2: 范围随机数 ===\n");

    random_seed(42);

    printf("生成范围内的随机整数:\n");
    printf("  random_range(1, 6) (骰子): ");
    for (int i = 0; i < 10; i++) {
        printf("%ld ", random_range(1, 6));
    }
    printf("\n");

    printf("  random_range(0, 100): ");
    for (int i = 0; i < 5; i++) {
        printf("%ld ", random_range(0, 100));
    }
    printf("\n");

    printf("\n生成范围内的随机浮点数:\n");
    printf("  random_double():     %.6f\n", random_double());
    printf("  random_double_range(0.0, 10.0): %.6f\n", random_double_range(0.0, 10.0));
    printf("  random_double_range(-5.0, 5.0): %.6f\n", random_double_range(-5.0, 5.0));
}

static void demo_bool_bytes(void) {
    printf("\n=== 演示 3: 布尔值和字节 ===\n");

    random_seed(100);

    printf("随机布尔值 (p=0.5):\n  ");
    for (int i = 0; i < 20; i++) {
        printf("%c", random_bool(0.5) ? '1' : '0');
    }
    printf("\n");

    printf("\n随机字节 (16字节):\n  ");
    uint8_t bytes[16];
    if (random_bytes(bytes, sizeof(bytes))) {
        for (int i = 0; i < 16; i++) {
            printf("%02x", bytes[i]);
        }
        printf("\n");
    }
}

static void demo_context(void) {
    printf("\n=== 演示 4: 上下文使用 ===\n");

    random_ctx_t ctx;
    random_error_t error;
    random_config_t config = random_default_config();

    printf("初始化 XORShift 生成器:\n");
    config.type = RANDOM_TYPE_XORSHIFT;
    config.seed = 12345;

    if (random_init(&ctx, &config, &error)) {
        printf("  成功!\n");
        printf("  random_u64_ctx(): %lu\n", random_u64_ctx(&ctx));
        printf("  random_u64_ctx(): %lu\n", random_u64_ctx(&ctx));
        printf("  random_double_ctx(): %.6f\n", random_double_ctx(&ctx));
    } else {
        printf("  初始化失败\n");
    }

    printf("\n初始化 LCG 生成器:\n");
    config.type = RANDOM_TYPE_LCG;
    config.seed = 54321;

    if (random_init(&ctx, &config, &error)) {
        printf("  成功!\n");
        printf("  random_u64_ctx(): %lu\n", random_u64_ctx(&ctx));
        printf("  random_u64_ctx(): %lu\n", random_u64_ctx(&ctx));
    }
}

static void demo_distribution(void) {
    printf("\n=== 演示 5: 分布统计 ===\n");

    random_seed(999);

    int histogram[10] = {0};
    int samples = 10000;

    printf("均匀分布测试 (%d 样本, 10 个区间):\n", samples);
    for (int i = 0; i < samples; i++) {
        int bin = (int)(random_double() * 10);
        if (bin >= 10) bin = 9;
        histogram[bin]++;
    }

    printf("  区间分布:\n");
    for (int i = 0; i < 10; i++) {
        printf("  [%d-%d]: %d (%.1f%%) ", i, i + 1, histogram[i], 
               histogram[i] * 100.0 / samples);
        int bars = histogram[i] / 25;
        for (int j = 0; j < bars; j++) printf("#");
        printf("\n");
    }
}

static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    random_seed(777);

    printf("1. 模拟掷骰子 100 次:\n");
    int dice_count[7] = {0};
    for (int i = 0; i < 100; i++) {
        int dice = (int)random_range(1, 6);
        dice_count[dice]++;
    }
    printf("   ");
    for (int i = 1; i <= 6; i++) {
        printf("%d:%d次 ", i, dice_count[i]);
    }
    printf("\n");

    printf("\n2. 生成随机密码 (12字符):\n   ");
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%";
    char password[13];
    for (int i = 0; i < 12; i++) {
        password[i] = charset[random_range(0, sizeof(charset) - 2)];
    }
    password[12] = '\0';
    printf("%s\n", password);

    printf("\n3. 随机打乱数组:\n");
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int n = sizeof(arr) / sizeof(arr[0]);
    printf("   原始: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    for (int i = n - 1; i > 0; i--) {
        int j = (int)random_range(0, i);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
    printf("   打乱: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    printf("========================================\n");
    printf("    随机数生成器演示\n");
    printf("========================================\n");

    demo_basic();
    demo_range();
    demo_bool_bytes();
    demo_context();
    demo_distribution();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
