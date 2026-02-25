/**
 * @file demo_bitstream.c
 * @brief 位流读取演示
 *
 * 本演示展示了位流读取的各种用法，包括：
 * - 基本位读取
 * - 多位读取
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bitstream.h"

// 打印二进制
static void print_binary(uint32_t value, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i > 0) printf(" ");
    }
}

// 演示 1: 基本位读取
static void demo_basic_read(void) {
    printf("\n=== 演示 1: 基本位读取 ===\n");

    // 创建测试数据: 0xAA, 0x55 (10101010 01010101)
    uint8_t data[] = {0xAA, 0x55};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("源数据: 0x%02X 0x%02X\n", data[0], data[1]);
    printf("二进制: 10101010 01010101\n\n");

    printf("逐位读取:\n");
    for (int i = 0; i < 16; i++) {
        uint32_t bit = bitstream_read(&bs, 1);
        printf("  位 %2d: %u\n", i, bit);
    }
}

// 演示 2: 多位读取
static void demo_multi_bit_read(void) {
    printf("\n=== 演示 2: 多位读取 ===\n");

    // 创建测试数据
    uint8_t data[] = {0xAB, 0xCD, 0xEF};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("源数据: 0x%02X 0x%02X 0x%02X\n", data[0], data[1], data[2]);
    printf("总位数: %zu\n\n", bs.len * 8);

    // 读取 4 位
    printf("读取 4 位组:\n");
    for (int i = 0; i < 6; i++) {
        uint32_t value = bitstream_read(&bs, 4);
        printf("  组 %d: ", i);
        print_binary(value, 4);
        printf(" (0x%X)\n", value);
    }
}

// 演示 3: 不同长度读取
static void demo_variable_length(void) {
    printf("\n=== 演示 3: 不同长度读取 ===\n");

    // 测试数据
    uint8_t data[] = {0xFF, 0x00, 0xAA, 0x55};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("源数据: 0x%02X 0x%02X 0x%02X 0x%02X\n\n",
           data[0], data[1], data[2], data[3]);

    // 读取不同长度的数据
    struct {
        int bits;
        const char* desc;
    } reads[] = {
        {1, "1 位"},
        {2, "2 位"},
        {3, "3 位"},
        {4, "4 位"},
        {5, "5 位"},
        {6, "6 位"},
        {7, "7 位"},
        {8, "8 位"}
    };

    printf("变长读取:\n");
    for (int i = 0; i < 8; i++) {
        uint32_t value = bitstream_read(&bs, reads[i].bits);
        printf("  %s: ", reads[i].desc);
        print_binary(value, reads[i].bits);
        printf(" (%u)\n", value);
    }
}

// 演示 4: 字节边界跨越
static void demo_cross_boundary(void) {
    printf("\n=== 演示 4: 字节边界跨越 ===\n");

    // 精心构造的数据
    uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("源数据: 0x12 0x34 0x56 0x78\n");
    printf("二进制: 00010010 00110100 01010110 01111000\n\n");

    // 读取跨越字节边界的数据
    printf("跨越字节边界读取:\n");

    // 读取 12 位（跨越第1、2字节）
    uint32_t val1 = bitstream_read(&bs, 12);
    printf("  12 位: ");
    print_binary(val1, 12);
    printf(" = 0x%03X\n", val1);

    // 读取 12 位（跨越第2、3字节）
    uint32_t val2 = bitstream_read(&bs, 12);
    printf("  12 位: ");
    print_binary(val2, 12);
    printf(" = 0x%03X\n", val2);

    // 读取 8 位
    uint32_t val3 = bitstream_read(&bs, 8);
    printf("  8 位:  ");
    print_binary(val3, 8);
    printf(" = 0x%02X\n", val3);
}

// 演示 5: 实际应用 - 哈夫曼编码解码
static void demo_huffman_decode(void) {
    printf("\n=== 演示 5: 哈夫曼编码解码模拟 ===\n");

    // 模拟哈夫曼编码的数据
    // 假设: 0 = A, 10 = B, 110 = C, 111 = D
    // 编码: A B A C D A B = 0 10 0 110 111 0 10
    // 二进制: 0100 1101 1101 0 = 0x4D D0 (部分字节)
    uint8_t data[] = {0x4D, 0xD0};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("哈夫曼编码数据: 0x4D 0xD0\n");
    printf("编码规则: 0=A, 10=B, 110=C, 111=D\n\n");

    printf("解码过程:\n");
    int bit_count = 0;
    while (bit_count < 13) {
        uint32_t bit = bitstream_read(&bs, 1);
        bit_count++;

        if (bit == 0) {
            printf("  读取 0 -> A\n");
        } else {
            uint32_t bit2 = bitstream_read(&bs, 1);
            bit_count++;

            if (bit2 == 0) {
                printf("  读取 10 -> B\n");
            } else {
                uint32_t bit3 = bitstream_read(&bs, 1);
                bit_count++;

                if (bit3 == 0) {
                    printf("  读取 110 -> C\n");
                } else {
                    printf("  读取 111 -> D\n");
                }
            }
        }
    }
}

// 演示 6: 实际应用 - 图像数据解析
static void demo_image_parse(void) {
    printf("\n=== 演示 6: 图像数据解析模拟 ===\n");

    // 模拟 BMP 文件头的一部分
    // BMP 文件头包含各种字段，有些不是字节对齐的
    uint8_t bmp_header[] = {
        0x42, 0x4D,             // 'BM'
        0x46, 0x00, 0x00, 0x00, // 文件大小
        0x00, 0x00,             // 保留
        0x00, 0x00,             // 保留
        0x36, 0x00, 0x00, 0x00  // 数据偏移
    };

    bitstream_t bs;
    bitstream_init(&bs, bmp_header, sizeof(bmp_header));

    printf("模拟 BMP 文件头解析:\n\n");

    // 读取签名 (16 位)
    uint32_t signature = bitstream_read(&bs, 16);
    printf("  签名: 0x%04X ('%c%c')\n",
           signature, (char)(signature & 0xFF), (char)(signature >> 8));

    // 读取文件大小 (32 位)
    uint32_t file_size = bitstream_read(&bs, 32);
    printf("  文件大小: %u 字节\n", file_size);

    // 读取保留字段 (32 位)
    uint32_t reserved = bitstream_read(&bs, 32);
    printf("  保留字段: 0x%08X\n", reserved);

    // 读取数据偏移 (32 位)
    uint32_t data_offset = bitstream_read(&bs, 32);
    printf("  数据偏移: %u 字节\n", data_offset);
}

// 演示 7: 实际应用 - 网络协议解析
static void demo_network_protocol(void) {
    printf("\n=== 演示 7: 网络协议解析模拟 ===\n");

    // 模拟 TCP 标志位（在 16 位中）
    // 位 0-3: 数据偏移 (4 位)
    // 位 4-9: 保留 (6 位)
    // 位 10: URG, 11: ACK, 12: PSH, 13: RST, 14: SYN, 15: FIN
    uint8_t tcp_flags[] = {0x50, 0x12}; // 数据偏移 5, SYN + ACK

    bitstream_t bs;
    bitstream_init(&bs, tcp_flags, sizeof(tcp_flags));

    printf("模拟 TCP 标志位解析:\n");
    printf("原始数据: 0x%02X 0x%02X\n\n", tcp_flags[0], tcp_flags[1]);

    uint32_t data_offset = bitstream_read(&bs, 4);
    printf("  数据偏移: %u (头部长度 %u 字节)\n", data_offset, data_offset * 4);

    uint32_t reserved = bitstream_read(&bs, 6);
    printf("  保留位: %u\n", reserved);

    printf("  标志位:\n");
    printf("    URG: %u\n", bitstream_read(&bs, 1));
    printf("    ACK: %u\n", bitstream_read(&bs, 1));
    printf("    PSH: %u\n", bitstream_read(&bs, 1));
    printf("    RST: %u\n", bitstream_read(&bs, 1));
    printf("    SYN: %u\n", bitstream_read(&bs, 1));
    printf("    FIN: %u\n", bitstream_read(&bs, 1));
}

// 演示 8: 位位置跟踪
static void demo_position_tracking(void) {
    printf("\n=== 演示 8: 位位置跟踪 ===\n");

    uint8_t data[] = {0xFF, 0xAA, 0x55, 0x00};
    bitstream_t bs;
    bitstream_init(&bs, data, sizeof(data));

    printf("源数据: 0xFF 0xAA 0x55 0x00\n");
    printf("总位数: %zu\n\n", bs.len * 8);

    printf("读取过程:\n");
    printf("  初始位置: %zu\n", bs.pos);

    bitstream_read(&bs, 8);
    printf("  读取 8 位后位置: %zu\n", bs.pos);

    bitstream_read(&bs, 4);
    printf("  读取 4 位后位置: %zu\n", bs.pos);

    bitstream_read(&bs, 12);
    printf("  读取 12 位后位置: %zu\n", bs.pos);

    bitstream_read(&bs, 8);
    printf("  读取 8 位后位置: %zu\n", bs.pos);

    printf("\n剩余位数: %zu\n", bs.len * 8 - bs.pos);
}

// 演示 9: 大端序读取
static void demo_endianness(void) {
    printf("\n=== 演示 9: 大端序读取 ===\n");

    // 大端序数据: 0x1234
    uint8_t big_endian[] = {0x12, 0x34};
    bitstream_t bs;
    bitstream_init(&bs, big_endian, sizeof(big_endian));

    printf("大端序数据: 0x12 0x34\n\n");

    // 读取 16 位
    uint32_t value = bitstream_read(&bs, 16);
    printf("读取 16 位: 0x%04X\n", value);
    printf("期望值: 0x1234\n");
    printf("结果: %s\n", value == 0x1234 ? "正确" : "错误");

    // 重新初始化
    bitstream_init(&bs, big_endian, sizeof(big_endian));

    // 逐字节读取
    printf("\n逐字节读取:\n");
    uint32_t byte1 = bitstream_read(&bs, 8);
    uint32_t byte2 = bitstream_read(&bs, 8);
    printf("  字节 1: 0x%02X\n", byte1);
    printf("  字节 2: 0x%02X\n", byte2);
}

// 演示 10: 实际应用 - 音频数据解析
static void demo_audio_parse(void) {
    printf("\n=== 演示 10: 音频数据解析模拟 ===\n");

    // 模拟音频采样数据（不同位深度）
    // 12 位采样数据打包在字节中
    uint8_t audio_data[] = {
        0x0A, 0xBC,  // 采样 1: 0xABC
        0x1D, 0xE0,  // 采样 2: 0x1DE (部分)
        0x2F, 0x30   // 采样 3: 0x2F3 (部分)
    };

    bitstream_t bs;
    bitstream_init(&bs, audio_data, sizeof(audio_data));

    printf("模拟 12 位音频采样解析:\n");
    printf("原始数据: ");
    for (size_t i = 0; i < sizeof(audio_data); i++) {
        printf("0x%02X ", audio_data[i]);
    }
    printf("\n\n");

    printf("解析采样:\n");
    for (int i = 0; i < 3 && bs.pos + 12 <= bs.len * 8; i++) {
        uint32_t sample = bitstream_read(&bs, 12);
        printf("  采样 %d: ", i + 1);
        print_binary(sample, 12);
        printf(" = 0x%03X = %u\n", sample, sample);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    位流读取演示程序\n");
    printf("========================================\n");

    demo_basic_read();
    demo_multi_bit_read();
    demo_variable_length();
    demo_cross_boundary();
    demo_huffman_decode();
    demo_image_parse();
    demo_network_protocol();
    demo_position_tracking();
    demo_endianness();
    demo_audio_parse();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
