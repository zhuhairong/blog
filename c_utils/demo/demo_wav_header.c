/**
 * WAV文件头处理演示程序
 *
 * 功能：
 * - WAV头初始化
 * - WAV头验证
 * - 从文件读取WAV头
 * - 写入WAV头到文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/wav_header.h"

// 演示 1: 基本WAV头初始化
static void demo_basic_init(void) {
    printf("\n=== 演示 1: 基本WAV头初始化 ===\n");

    // 创建WAV头上下文
    wav_hdr_ctx_t *ctx = NULL;
    wav_hdr_config_t config = {
        .strict_validation = true,
        .allow_non_pcm = false,
        .check_file_size = true,
        .max_header_size = 1024
    };

    wav_hdr_error_t error = wav_hdr_create(&ctx, &config);
    if (error != WAV_HDR_OK) {
        printf("创建WAV头上下文失败: %s\n", wav_hdr_strerror(error));
        return;
    }

    // 初始化WAV头
    wav_hdr_t hdr;
    error = wav_hdr_init_safe(ctx, &hdr, 44100, 2, 16, 44100 * 2 * 2 * 5); // 5秒立体声
    if (error == WAV_HDR_OK) {
        printf("WAV头初始化成功\n");
        printf("  采样率: %u Hz\n", hdr.sample_rate);
        printf("  声道数: %u\n", hdr.num_channels);
        printf("  位深度: %u bits\n", hdr.bits_per_sample);
        printf("  音频格式: %u (%s)\n", hdr.audio_format, wav_hdr_get_format_name(hdr.audio_format));
        printf("  数据大小: %u 字节\n", hdr.subchunk2_size);
        printf("  总文件大小: %u 字节\n", wav_hdr_get_file_size(&hdr));
        printf("  是否为PCM格式: %s\n", wav_hdr_is_pcm(&hdr) ? "是" : "否");
    } else {
        printf("WAV头初始化失败: %s\n", wav_hdr_strerror(error));
    }

    wav_hdr_destroy(ctx);
}

// 演示 2: WAV头验证
static void demo_validation(void) {
    printf("\n=== 演示 2: WAV头验证 ===\n");

    // 创建WAV头上下文
    wav_hdr_ctx_t *ctx = NULL;
    wav_hdr_config_t config = {
        .strict_validation = true,
        .allow_non_pcm = false,
        .check_file_size = true,
        .max_header_size = 1024
    };

    wav_hdr_error_t error = wav_hdr_create(&ctx, &config);
    if (error != WAV_HDR_OK) {
        printf("创建WAV头上下文失败: %s\n", wav_hdr_strerror(error));
        return;
    }

    // 初始化有效WAV头
    wav_hdr_t valid_hdr;
    error = wav_hdr_init_safe(ctx, &valid_hdr, 44100, 2, 16, 44100 * 2 * 2);
    if (error == WAV_HDR_OK) {
        // 验证有效WAV头
        error = wav_hdr_validate(ctx, &valid_hdr);
        if (error == WAV_HDR_OK) {
            printf("有效WAV头验证成功\n");
        } else {
            printf("有效WAV头验证失败: %s\n", wav_hdr_strerror(error));
        }

        // 创建无效WAV头（修改格式ID）
        wav_hdr_t invalid_hdr = valid_hdr;
        invalid_hdr.format[0] = 'X'; // 破坏格式ID

        // 验证无效WAV头
        error = wav_hdr_validate(ctx, &invalid_hdr);
        if (error != WAV_HDR_OK) {
            printf("无效WAV头验证失败 (预期): %s\n", wav_hdr_strerror(error));
        } else {
            printf("无效WAV头验证成功 (意外)\n");
        }
    }

    wav_hdr_destroy(ctx);
}

// 演示 3: WAV头文件操作
static void demo_file_operations(void) {
    printf("\n=== 演示 3: WAV头文件操作 ===\n");

    const char *filename = "/tmp/test.wav";

    // 创建WAV头上下文
    wav_hdr_ctx_t *ctx = NULL;
    wav_hdr_config_t config = {
        .strict_validation = true,
        .allow_non_pcm = false,
        .check_file_size = true,
        .max_header_size = 1024
    };

    wav_hdr_error_t error = wav_hdr_create(&ctx, &config);
    if (error != WAV_HDR_OK) {
        printf("创建WAV头上下文失败: %s\n", wav_hdr_strerror(error));
        return;
    }

    // 初始化WAV头
    wav_hdr_t hdr;
    error = wav_hdr_init_safe(ctx, &hdr, 44100, 2, 16, 44100 * 2 * 2 * 2); // 2秒立体声
    if (error == WAV_HDR_OK) {
        // 写入WAV头到文件
        error = wav_hdr_write(ctx, filename, &hdr);
        if (error == WAV_HDR_OK) {
            printf("WAV头写入文件成功: %s\n", filename);

            // 从文件读取WAV头
            wav_hdr_t read_hdr;
            error = wav_hdr_read(ctx, filename, &read_hdr);
            if (error == WAV_HDR_OK) {
                printf("从文件读取WAV头成功\n");
                printf("  采样率: %u Hz\n", read_hdr.sample_rate);
                printf("  声道数: %u\n", read_hdr.num_channels);
                printf("  位深度: %u bits\n", read_hdr.bits_per_sample);
                printf("  数据大小: %u 字节\n", read_hdr.subchunk2_size);
            } else {
                printf("从文件读取WAV头失败: %s\n", wav_hdr_strerror(error));
            }
        } else {
            printf("WAV头写入文件失败: %s\n", wav_hdr_strerror(error));
        }
    }

    // 清理文件
    remove(filename);
    wav_hdr_destroy(ctx);
}

// 演示 4: 数据大小操作
static void demo_data_size(void) {
    printf("\n=== 演示 4: 数据大小操作 ===\n");

    // 创建WAV头上下文
    wav_hdr_ctx_t *ctx = NULL;
    wav_hdr_config_t config = {
        .strict_validation = true,
        .allow_non_pcm = false,
        .check_file_size = true,
        .max_header_size = 1024
    };

    wav_hdr_error_t error = wav_hdr_create(&ctx, &config);
    if (error != WAV_HDR_OK) {
        printf("创建WAV头上下文失败: %s\n", wav_hdr_strerror(error));
        return;
    }

    // 初始化WAV头
    wav_hdr_t hdr;
    error = wav_hdr_init_safe(ctx, &hdr, 44100, 2, 16, 44100 * 2 * 2); // 1秒立体声
    if (error == WAV_HDR_OK) {
        printf("初始数据大小: %u 字节\n", wav_hdr_get_data_size(&hdr));
        printf("初始文件大小: %u 字节\n", wav_hdr_get_file_size(&hdr));

        // 修改数据大小
        uint32_t new_data_size = 44100 * 2 * 2 * 5; // 5秒
        error = wav_hdr_set_data_size(ctx, &hdr, new_data_size);
        if (error == WAV_HDR_OK) {
            printf("修改后数据大小: %u 字节\n", wav_hdr_get_data_size(&hdr));
            printf("修改后文件大小: %u 字节\n", wav_hdr_get_file_size(&hdr));
        } else {
            printf("修改数据大小失败: %s\n", wav_hdr_strerror(error));
        }
    }

    wav_hdr_destroy(ctx);
}

// 演示 5: 不同格式测试
static void demo_formats(void) {
    printf("\n=== 演示 5: 不同格式测试 ===\n");

    // 测试不同的音频格式
    uint16_t formats[] = {1, 2, 3, 6, 7, 17};
    size_t formats_count = sizeof(formats) / sizeof(formats[0]);

    for (size_t i = 0; i < formats_count; i++) {
        uint16_t format = formats[i];
        printf("格式 %u: %s\n", format, wav_hdr_get_format_name(format));
    }
}

// 演示 6: 传统API使用
static void demo_legacy_api(void) {
    printf("\n=== 演示 6: 传统API使用 ===\n");

    // 使用传统API初始化WAV头
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 44100 * 2 * 2 * 3); // 3秒立体声

    printf("传统API初始化WAV头\n");
    printf("  采样率: %u Hz\n", hdr.sample_rate);
    printf("  声道数: %u\n", hdr.num_channels);
    printf("  位深度: %u bits\n", hdr.bits_per_sample);
    printf("  数据大小: %u 字节\n", hdr.subchunk2_size);
}

int main(void) {
    printf("========================================\n");
    printf("    WAV文件头处理演示\n");
    printf("========================================\n");

    demo_basic_init();
    demo_validation();
    demo_file_operations();
    demo_data_size();
    demo_formats();
    demo_legacy_api();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
