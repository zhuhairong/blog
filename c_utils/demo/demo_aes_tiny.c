/**
 * @file demo_aes_tiny.c
 * @brief AES 轻量级加密算法演示
 *
 * 本演示展示了 AES 轻量级加密算法的各种用法，包括：
 * - AES-128/192/256 加密和解密
 * - ECB/CBC/CFB/OFB 模式
 * - 块加密和批量加密
 * - 不同密钥长度的使用
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../c_utils/aes_tiny.h"

// 打印十六进制数据
static void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
        if ((i + 1) % 16 == 0 && i < len - 1) printf(" ");
    }
    printf("\n");
}

// 打印错误信息
static void print_error(const char* operation, aes_tiny_error_t error) {
    fprintf(stderr, "操作 '%s' 失败: %s\n", operation, aes_tiny_strerror(error));
}

// 演示 1: 简单加密（向后兼容）
static void demo_simple_encrypt(void) {
    printf("\n=== 演示 1: 简单块加密（向后兼容） ===\n");

    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    uint8_t plaintext[16] = "Hello, AES-128!";
    uint8_t ciphertext[16];

    printf("明文: \"%s\"\n", plaintext);
    print_hex("密钥", key, 16);

    aes_tiny_encrypt_simple(plaintext, key, ciphertext);
    print_hex("密文", ciphertext, 16);
}

// 演示 2: AES-128 CBC 模式加密解密
static void demo_aes128_cbc(void) {
    printf("\n=== 演示 2: AES-128 CBC 模式加密解密 ===\n");

    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_CBC,
        .key_size = AES_TINY_KEY_SIZE_128
    };

    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    uint8_t iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    if (err != AES_TINY_OK) {
        print_error("创建 AES-128 上下文", err);
        return;
    }

    const char* plaintext = "Hello, AES-128 CBC Mode Encryption!";
    size_t plaintext_len = strlen(plaintext);
    uint8_t ciphertext[64];
    uint8_t decrypted[64];
    size_t ciphertext_len, decrypted_len;

    printf("明文: \"%s\"\n", plaintext);
    print_hex("密钥", key, 16);
    print_hex("IV", iv, 16);

    // 加密
    err = aes_tiny_encrypt(ctx, (const uint8_t*)plaintext, plaintext_len,
                           ciphertext, &ciphertext_len, iv);
    if (err != AES_TINY_OK) {
        print_error("加密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    print_hex("密文", ciphertext, ciphertext_len);
    printf("密文长度: %zu\n", ciphertext_len);

    // 解密
    err = aes_tiny_decrypt(ctx, ciphertext, ciphertext_len,
                           decrypted, &decrypted_len, iv);
    if (err != AES_TINY_OK) {
        print_error("解密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    decrypted[decrypted_len] = '\0';
    printf("解密结果: \"%s\"\n", decrypted);
    printf("解密长度: %zu\n", decrypted_len);

    aes_tiny_destroy(ctx);
}

// 演示 3: AES-256 CBC 模式
static void demo_aes256_cbc(void) {
    printf("\n=== 演示 3: AES-256 CBC 模式加密解密 ===\n");

    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_CBC,
        .key_size = AES_TINY_KEY_SIZE_256
    };

    uint8_t key[32] = {
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    uint8_t iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 32);
    if (err != AES_TINY_OK) {
        print_error("创建 AES-256 上下文", err);
        return;
    }

    const char* plaintext = "AES-256 provides stronger encryption!";
    size_t plaintext_len = strlen(plaintext);
    uint8_t ciphertext[64];
    uint8_t decrypted[64];
    size_t ciphertext_len, decrypted_len;

    printf("明文: \"%s\"\n", plaintext);
    print_hex("密钥", key, 32);

    err = aes_tiny_encrypt(ctx, (const uint8_t*)plaintext, plaintext_len,
                           ciphertext, &ciphertext_len, iv);
    if (err != AES_TINY_OK) {
        print_error("加密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    print_hex("密文", ciphertext, ciphertext_len);

    err = aes_tiny_decrypt(ctx, ciphertext, ciphertext_len,
                           decrypted, &decrypted_len, iv);
    if (err != AES_TINY_OK) {
        print_error("解密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    decrypted[decrypted_len] = '\0';
    printf("解密结果: \"%s\"\n", decrypted);

    aes_tiny_destroy(ctx);
}

// 演示 4: 不同加密模式对比
static void demo_modes_comparison(void) {
    printf("\n=== 演示 4: 不同加密模式对比 ===\n");

    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    uint8_t iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    const char* plaintext = "Testing different AES modes!";
    size_t plaintext_len = strlen(plaintext);

    aes_tiny_mode_t modes[] = {
        AES_TINY_MODE_ECB,
        AES_TINY_MODE_CBC,
        AES_TINY_MODE_CFB,
        AES_TINY_MODE_OFB
    };
    const char* mode_names[] = {"ECB", "CBC", "CFB", "OFB"};

    for (int i = 0; i < 4; i++) {
        printf("\n--- %s 模式 ---\n", mode_names[i]);

        aes_tiny_ctx_t* ctx = NULL;
        aes_tiny_config_t config = {
            .use_padding = true,
            .check_key_size = true,
            .check_block_size = true,
            .mode = modes[i],
            .key_size = AES_TINY_KEY_SIZE_128
        };

        aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
        if (err != AES_TINY_OK) {
            print_error("创建上下文", err);
            continue;
        }

        uint8_t ciphertext[64];
        uint8_t decrypted[64];
        size_t ciphertext_len, decrypted_len;

        err = aes_tiny_encrypt(ctx, (const uint8_t*)plaintext, plaintext_len,
                               ciphertext, &ciphertext_len, iv);
        if (err != AES_TINY_OK) {
            print_error("加密", err);
            aes_tiny_destroy(ctx);
            continue;
        }
        print_hex("密文", ciphertext, ciphertext_len);

        err = aes_tiny_decrypt(ctx, ciphertext, ciphertext_len,
                               decrypted, &decrypted_len, iv);
        if (err != AES_TINY_OK) {
            print_error("解密", err);
            aes_tiny_destroy(ctx);
            continue;
        }
        decrypted[decrypted_len] = '\0';
        printf("解密: \"%s\"\n", decrypted);

        aes_tiny_destroy(ctx);
    }
}

// 演示 5: 单个块加密解密
static void demo_single_block(void) {
    printf("\n=== 演示 5: 单个块加密解密 ===\n");

    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_config_t config = {
        .use_padding = false,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_ECB,
        .key_size = AES_TINY_KEY_SIZE_128
    };

    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };

    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    if (err != AES_TINY_OK) {
        print_error("创建上下文", err);
        return;
    }

    uint8_t block[16] = "SingleBlockTest!";
    uint8_t encrypted[16];
    uint8_t decrypted[16];

    printf("原始块: \"%s\"\n", block);
    print_hex("原始块 (hex)", block, 16);

    err = aes_tiny_encrypt_block(ctx, block, encrypted);
    if (err != AES_TINY_OK) {
        print_error("块加密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    print_hex("加密后", encrypted, 16);

    err = aes_tiny_decrypt_block(ctx, encrypted, decrypted);
    if (err != AES_TINY_OK) {
        print_error("块解密", err);
        aes_tiny_destroy(ctx);
        return;
    }
    print_hex("解密后", decrypted, 16);
    printf("解密结果: \"%s\"\n", decrypted);

    aes_tiny_destroy(ctx);
}

// 演示 6: 批量加密
static void demo_batch_encrypt(void) {
    printf("\n=== 演示 6: 批量加密 ===\n");

    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_CBC,
        .key_size = AES_TINY_KEY_SIZE_128
    };

    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    uint8_t iv[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    if (err != AES_TINY_OK) {
        print_error("创建上下文", err);
        return;
    }

    const char* messages[] = {
        "First message",
        "Second message",
        "Third message"
    };
    size_t num_messages = 3;

    const uint8_t* inputs[3];
    size_t input_lens[3];
    uint8_t* outputs[3];
    size_t output_lens[3];

    printf("批量加密 %zu 条消息:\n", num_messages);
    for (size_t i = 0; i < num_messages; i++) {
        inputs[i] = (const uint8_t*)messages[i];
        input_lens[i] = strlen(messages[i]);
        outputs[i] = malloc(64);
        printf("  消息 %zu: \"%s\"\n", i + 1, messages[i]);
    }

    err = aes_tiny_encrypt_batch(ctx, inputs, input_lens, num_messages,
                                  outputs, output_lens, iv);
    if (err != AES_TINY_OK) {
        print_error("批量加密", err);
        aes_tiny_destroy(ctx);
        for (size_t i = 0; i < num_messages; i++) free(outputs[i]);
        return;
    }

    printf("\n加密结果:\n");
    for (size_t i = 0; i < num_messages; i++) {
        printf("  消息 %zu: ", i + 1);
        for (size_t j = 0; j < output_lens[i]; j++) {
            printf("%02X", outputs[i][j]);
        }
        printf("\n");
        free(outputs[i]);
    }

    aes_tiny_destroy(ctx);
}

// 演示 7: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_error_t err;

    // 测试无效密钥大小
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_CBC,
        .key_size = AES_TINY_KEY_SIZE_128
    };
    uint8_t invalid_key[8] = {0};
    err = aes_tiny_create(&ctx, &config, invalid_key, 8);
    printf("使用 8 字节密钥创建 AES-128 上下文: %s\n", aes_tiny_strerror(err));

    // 测试不支持的加密模式
    config.mode = 99;  // 无效模式
    uint8_t key[16] = {0};
    err = aes_tiny_create(&ctx, &config, key, 16);
    printf("使用无效加密模式: %s\n", aes_tiny_strerror(err));
}

int main(void) {
    printf("========================================\n");
    printf("    AES 轻量级加密算法演示程序\n");
    printf("========================================\n");

    demo_simple_encrypt();
    demo_aes128_cbc();
    demo_aes256_cbc();
    demo_modes_comparison();
    demo_single_block();
    demo_batch_encrypt();
    demo_error_handling();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
