#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url_codec.h"

void demo_url_encode(void) {
    printf("\n=== URL编码演示 ===\n");
    
    url_codec_ctx_t* ctx = NULL;
    url_codec_config_t config = {
        .use_plus_for_space = false,
        .encode_reserved_chars = false,
        .strict_mode = false,
        .max_input_length = 1024
    };
    
    url_codec_error_t err = url_codec_create(&ctx, &config);
    if (err != URL_CODEC_OK) {
        printf("创建上下文失败\n");
        return;
    }
    
    const char* test_strings[] = {
        "hello world",
        "user@example.com",
        "a/b/c",
        "name=value&key=data"
    };
    
    for (int i = 0; i < 4; i++) {
        size_t in_len = strlen(test_strings[i]);
        size_t out_size = 0;
        
        err = url_codec_encode(ctx, test_strings[i], in_len, NULL, &out_size);
        if (err == URL_CODEC_OK) {
            char* encoded = (char*)malloc(out_size);
            if (encoded) {
                err = url_codec_encode(ctx, test_strings[i], in_len, encoded, &out_size);
                if (err == URL_CODEC_OK) {
                    printf("原始: %s\n", test_strings[i]);
                    printf("编码: %s\n", encoded);
                }
                free(encoded);
            }
        }
    }
    
    url_codec_destroy(ctx);
}

void demo_url_decode(void) {
    printf("\n=== URL解码演示 ===\n");
    
    url_codec_ctx_t* ctx = NULL;
    url_codec_error_t err = url_codec_create(&ctx, NULL);
    if (err != URL_CODEC_OK) {
        printf("创建上下文失败\n");
        return;
    }
    
    const char* encoded_strings[] = {
        "hello%20world",
        "user%40example.com",
        "a%2Fb%2Fc",
        "name%3Dvalue%26key%3Ddata"
    };
    
    for (int i = 0; i < 4; i++) {
        size_t in_len = strlen(encoded_strings[i]);
        size_t out_size = 0;
        
        err = url_codec_decode(ctx, encoded_strings[i], in_len, NULL, &out_size);
        if (err == URL_CODEC_OK) {
            char* decoded = (char*)malloc(out_size);
            if (decoded) {
                err = url_codec_decode(ctx, encoded_strings[i], in_len, decoded, &out_size);
                if (err == URL_CODEC_OK) {
                    printf("编码: %s\n", encoded_strings[i]);
                    printf("解码: %s\n", decoded);
                }
                free(decoded);
            }
        }
    }
    
    url_codec_destroy(ctx);
}

void demo_url_path_encoding(void) {
    printf("\n=== URL路径编码演示 ===\n");
    
    url_codec_ctx_t* ctx = NULL;
    url_codec_error_t err = url_codec_create(&ctx, NULL);
    if (err != URL_CODEC_OK) {
        printf("创建上下文失败\n");
        return;
    }
    
    const char* paths[] = {
        "/path/to/file.txt",
        "/user name/document.pdf",
        "/data/2024/01/file.csv"
    };
    
    for (int i = 0; i < 3; i++) {
        size_t out_size = 0;
        
        err = url_codec_encode_path(ctx, paths[i], NULL, &out_size);
        if (err == URL_CODEC_OK) {
            char* encoded = (char*)malloc(out_size);
            if (encoded) {
                err = url_codec_encode_path(ctx, paths[i], encoded, &out_size);
                if (err == URL_CODEC_OK) {
                    printf("路径: %s\n", paths[i]);
                    printf("编码: %s\n", encoded);
                }
                free(encoded);
            }
        }
    }
    
    url_codec_destroy(ctx);
}

void demo_legacy_api(void) {
    printf("\n=== 传统API演示 ===\n");
    
    const char* test_str = "hello world";
    char encoded[256] = {0};
    char decoded[256] = {0};
    
    printf("原始: %s\n", test_str);
    
    url_encode(test_str, encoded);
    printf("编码: %s\n", encoded);
    
    url_decode(encoded, decoded);
    printf("解码: %s\n", decoded);
}

void demo_special_characters(void) {
    printf("\n=== 特殊字符编码演示 ===\n");
    
    url_codec_ctx_t* ctx = NULL;
    url_codec_error_t err = url_codec_create(&ctx, NULL);
    if (err != URL_CODEC_OK) {
        printf("创建上下文失败\n");
        return;
    }
    
    const char* special_chars[] = {
        "!@#$%^&*()",
        "中文测试",
        "<html>&amp;</html>",
        "line1\nline2\ttab"
    };
    
    for (int i = 0; i < 4; i++) {
        size_t in_len = strlen(special_chars[i]);
        size_t out_size = 0;
        
        err = url_codec_encode(ctx, special_chars[i], in_len, NULL, &out_size);
        if (err == URL_CODEC_OK) {
            char* encoded = (char*)malloc(out_size);
            if (encoded) {
                err = url_codec_encode(ctx, special_chars[i], in_len, encoded, &out_size);
                if (err == URL_CODEC_OK) {
                    printf("原始: %s\n", special_chars[i]);
                    printf("编码: %s\n", encoded);
                }
                free(encoded);
            }
        }
    }
    
    url_codec_destroy(ctx);
}

int main(void) {
    printf("========================================\n");
    printf("    URL编解码演示程序\n");
    printf("========================================\n");
    
    demo_url_encode();
    demo_url_decode();
    demo_url_path_encoding();
    demo_legacy_api();
    demo_special_characters();
    
    printf("\n=== URL编解码演示完成 ===\n");
    return 0;
}
