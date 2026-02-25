/**
 * RLE (游程编码) 演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/rle.h"

static void demo_encode(void) {
    printf("\n=== 演示 1: RLE 编码 ===\n");

    const char *inputs[] = {
        "AAAAAABBBBCCDDDDD",
        "Hello World",
        "11222333444",
        "XXXXXXXXXX"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char *input = inputs[i];
        size_t in_len = strlen(input);
        unsigned char output[256];
        
        size_t out_len = rle_encode((const unsigned char*)input, in_len, output);
        
        printf("输入: \"%s\" (%zu 字节)\n", input, in_len);
        printf("编码: ");
        for (size_t j = 0; j < out_len; j++) {
            printf("%02X ", output[j]);
        }
        printf("(%zu 字节)\n\n", out_len);
    }
}

static void demo_decode(void) {
    printf("\n=== 演示 2: RLE 解码 ===\n");

    const char *original = "AAAAAABBBBCC";
    unsigned char encoded[256];
    unsigned char decoded[256];
    
    size_t enc_len = rle_encode((const unsigned char*)original, strlen(original), encoded);
    size_t dec_len = rle_decode(encoded, enc_len, decoded);
    decoded[dec_len] = '\0';
    
    printf("原始: \"%s\" (%zu 字节)\n", original, strlen(original));
    printf("编码后: %zu 字节\n", enc_len);
    printf("解码: \"%s\" (%zu 字节)\n", decoded, dec_len);
    printf("验证: %s\n", strcmp(original, (char*)decoded) == 0 ? "成功" : "失败");
}

static void demo_compression_ratio(void) {
    printf("\n=== 演示 3: 压缩率分析 ===\n");

    struct {
        const char *name;
        const char *data;
    } test_cases[] = {
        {"重复数据", "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC"},
        {"随机数据", "ABCDEFGHIJ"},
        {"混合数据", "AABBBCCCCDDDDDEEEEEE"},
        {"二值数据", "000000001111111100000000"}
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("%-12s %10s %10s %10s\n", "类型", "原始", "编码后", "压缩率");
    printf("----------------------------------------------\n");

    for (int i = 0; i < num_cases; i++) {
        size_t in_len = strlen(test_cases[i].data);
        unsigned char output[256];
        size_t out_len = rle_encode((const unsigned char*)test_cases[i].data, in_len, output);
        
        double ratio = (double)out_len / in_len * 100;
        printf("%-12s %10zu %10zu %9.1f%%\n", 
               test_cases[i].name, in_len, out_len, ratio);
    }
}

static void demo_roundtrip(void) {
    printf("\n=== 演示 4: 完整编解码测试 ===\n");

    const char *test_data = "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWAAAAAABBBBCC";
    unsigned char encoded[256];
    unsigned char decoded[256];
    
    printf("原始数据: \"%s\"\n", test_data);
    printf("原始长度: %zu 字节\n\n", strlen(test_data));
    
    size_t enc_len = rle_encode((const unsigned char*)test_data, strlen(test_data), encoded);
    printf("编码长度: %zu 字节\n", enc_len);
    printf("编码数据: ");
    for (size_t i = 0; i < enc_len && i < 20; i++) {
        printf("%02X ", encoded[i]);
    }
    if (enc_len > 20) printf("...");
    printf("\n\n");
    
    size_t dec_len = rle_decode(encoded, enc_len, decoded);
    decoded[dec_len] = '\0';
    printf("解码长度: %zu 字节\n", dec_len);
    printf("解码数据: \"%s\"\n\n", decoded);
    
    bool match = (dec_len == strlen(test_data)) && 
                 (memcmp(test_data, decoded, dec_len) == 0);
    printf("验证结果: %s\n", match ? "编解码正确" : "编解码错误");
}

int main(void) {
    printf("========================================\n");
    printf("    RLE (游程编码) 演示\n");
    printf("========================================\n");

    demo_encode();
    demo_decode();
    demo_compression_ratio();
    demo_roundtrip();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
