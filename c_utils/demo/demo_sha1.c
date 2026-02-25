/**
 * SHA1 哈希演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/sha1.h"

static void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 SHA1 计算 ===\n");

    const char *inputs[] = {
        "",
        "hello",
        "Hello World",
        "The quick brown fox jumps over the lazy dog"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        sha1_ctx_t ctx;
        uint8_t digest[20];
        
        sha1_init(&ctx);
        sha1_update(&ctx, (const uint8_t*)inputs[i], strlen(inputs[i]));
        sha1_final(&ctx, digest);
        
        printf("输入: \"%s\"\n", inputs[i][0] ? inputs[i] : "(空)");
        printf("SHA1: ");
        print_hex(digest, 20);
        printf("\n\n");
    }
}

static void demo_incremental(void) {
    printf("\n=== 演示 2: 增量计算 ===\n");

    sha1_ctx_t ctx;
    uint8_t digest[20];

    printf("分块计算 SHA1:\n\n");
    sha1_init(&ctx);

    const char *parts[] = {"Hello ", "World", "!"};
    for (int i = 0; i < 3; i++) {
        sha1_update(&ctx, (const uint8_t*)parts[i], strlen(parts[i]));
        printf("  更新: \"%s\"\n", parts[i]);
    }

    sha1_final(&ctx, digest);
    printf("\n最终 SHA1: ");
    print_hex(digest, 20);
    printf("\n");

    printf("\n验证 (一次性计算):\n");
    sha1_ctx_t ctx2;
    uint8_t digest2[20];
    sha1_init(&ctx2);
    sha1_update(&ctx2, (const uint8_t*)"Hello World!", 12);
    sha1_final(&ctx2, digest2);
    printf("SHA1: ");
    print_hex(digest2, 20);
    printf("\n");

    bool match = memcmp(digest, digest2, 20) == 0;
    printf("\n结果: %s\n", match ? "匹配" : "不匹配");
}

static void demo_properties(void) {
    printf("\n=== 演示 3: SHA1 特性 ===\n");

    printf("1. 固定输出长度 (160 位 / 20 字节)\n");
    sha1_ctx_t ctx;
    uint8_t digest[20];
    
    sha1_init(&ctx);
    sha1_update(&ctx, (const uint8_t*)"short", 5);
    sha1_final(&ctx, digest);
    printf("   短输入: ");
    print_hex(digest, 20);
    printf("\n");

    sha1_init(&ctx);
    const char *long_input = "This is a much longer input string to demonstrate that SHA1 always produces the same length output regardless of input size.";
    sha1_update(&ctx, (const uint8_t*)long_input, strlen(long_input));
    sha1_final(&ctx, digest);
    printf("   长输入: ");
    print_hex(digest, 20);
    printf("\n\n");

    printf("2. 雪崩效应 (微小变化导致完全不同的输出)\n");
    sha1_init(&ctx);
    sha1_update(&ctx, (const uint8_t*)"Hello", 5);
    sha1_final(&ctx, digest);
    printf("   \"Hello\":  ");
    print_hex(digest, 20);
    printf("\n");

    sha1_init(&ctx);
    sha1_update(&ctx, (const uint8_t*)"hello", 5);
    sha1_final(&ctx, digest);
    printf("   \"hello\":  ");
    print_hex(digest, 20);
    printf("\n");
}

int main(void) {
    printf("========================================\n");
    printf("    SHA1 哈希演示\n");
    printf("========================================\n");

    demo_basic();
    demo_incremental();
    demo_properties();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
