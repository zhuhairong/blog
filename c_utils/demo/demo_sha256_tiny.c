/**
 * SHA256 精简版演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/sha256_tiny.h"

static void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 SHA256 计算 ===\n");

    const char *inputs[] = {
        "",
        "hello",
        "Hello World",
        "The quick brown fox jumps over the lazy dog"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        uint8_t digest[32];
        
        sha256_tiny((const uint8_t*)inputs[i], strlen(inputs[i]), digest);
        
        printf("输入: \"%s\"\n", inputs[i][0] ? inputs[i] : "(空)");
        printf("SHA256: ");
        print_hex(digest, 32);
        printf("\n\n");
    }
}

static void demo_properties(void) {
    printf("\n=== 演示 2: SHA256 特性 ===\n");

    printf("1. 固定输出长度 (256 位 / 32 字节)\n");
    uint8_t digest[32];
    
    sha256_tiny((const uint8_t*)"short", 5, digest);
    printf("   短输入: ");
    print_hex(digest, 32);
    printf("\n");

    const char *long_input = "This is a much longer input string";
    sha256_tiny((const uint8_t*)long_input, strlen(long_input), digest);
    printf("   长输入: ");
    print_hex(digest, 32);
    printf("\n\n");

    printf("2. 雪崩效应\n");
    sha256_tiny((const uint8_t*)"Hello", 5, digest);
    printf("   \"Hello\":  ");
    print_hex(digest, 32);
    printf("\n");

    sha256_tiny((const uint8_t*)"hello", 5, digest);
    printf("   \"hello\":  ");
    print_hex(digest, 32);
    printf("\n");
}

int main(void) {
    printf("========================================\n");
    printf("    SHA256 精简版演示\n");
    printf("========================================\n");

    demo_basic();
    demo_properties();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
