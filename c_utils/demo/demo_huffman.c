/**
 * 哈夫曼编码演示程序
 *
 * 功能：
 * - 频率统计
 * - 哈夫曼树构建
 * - 编码和解码
 * - 压缩率分析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/huffman.h"

// 打印编码表
static void print_code_table(huffman_code_t table[256], size_t freq[256]) {
    printf("哈夫曼编码表:\n");
    printf("  %-8s %-10s %-15s\n", "字符", "频率", "编码");
    printf("  %-8s %-10s %-15s\n", "--------", "----------", "---------------");

    for (int i = 0; i < 256; i++) {
        if (table[i].code_len > 0) {
            printf("  %-8c %-10zu ", (char)i, freq[i]);
            // 打印二进制编码
            for (int j = table[i].code_len - 1; j >= 0; j--) {
                printf("%d", (table[i].code >> j) & 1);
            }
            printf("\n");
        }
    }
}

// 演示 1: 频率统计
static void demo_frequency(void) {
    printf("\n=== 演示 1: 字符频率统计 ===\n");

    const char *text = "hello world, this is a huffman coding example!";
    size_t freq[256] = {0};

    printf("原文: %s\n\n", text);

    huffman_stats((const unsigned char*)text, strlen(text), freq);

    printf("字符频率统计:\n");
    printf("  %-8s %-10s\n", "字符", "频率");
    printf("  %-8s %-10s\n", "--------", "----------");

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            if (i == ' ') {
                printf("  %-8s %-10zu\n", "[空格]", freq[i]);
            } else if (i == '\n') {
                printf("  %-8s %-10zu\n", "[换行]", freq[i]);
            } else {
                printf("  %-8c %-10zu\n", (char)i, freq[i]);
            }
        }
    }
}

// 演示 2: 哈夫曼树构建
static void demo_build_tree(void) {
    printf("\n=== 演示 2: 构建哈夫曼树 ===\n");

    const char *text = "aaabbc";
    size_t freq[256] = {0};

    printf("示例文本: %s\n\n", text);

    huffman_stats((const unsigned char*)text, strlen(text), freq);

    printf("字符频率:\n");
    printf("  'a': %zu\n", freq['a']);
    printf("  'b': %zu\n", freq['b']);
    printf("  'c': %zu\n", freq['c']);

    printf("\n哈夫曼树构建过程:\n");
    printf("  1. 为每个字符创建叶子节点\n");
    printf("  2. 每次选择频率最小的两个节点合并\n");
    printf("  3. 新节点频率 = 子节点频率之和\n");
    printf("  4. 重复直到只剩一个根节点\n");

    printf("\n本例构建过程:\n");
    printf("  初始: a(3), b(2), c(1)\n");
    printf("  步骤 1: 合并 b(2) + c(1) = 节点(3)\n");
    printf("  步骤 2: 合并 a(3) + 节点(3) = 根节点(6)\n");
    printf("  完成: 哈夫曼树构建完成\n");
}

// 演示 3: 编码和解码
static void demo_encode_decode(void) {
    printf("\n=== 演示 3: 编码和解码 ===\n");

    huffman_error_t error;

    const char *text = "hello huffman";
    size_t len = strlen(text);

    printf("原文: %s\n", text);
    printf("原文长度: %zu 字节 (%zu 位)\n\n", len, len * 8);

    // 统计频率
    size_t freq[256] = {0};
    huffman_stats((const unsigned char*)text, len, freq);

    // 创建哈夫曼树
    huffman_node_t *root = huffman_create_tree(freq, &error);
    if (!root) {
        printf("创建树失败\n");
        return;
    }

    // 构建编码表
    huffman_code_t table[256];
    memset(table, 0, sizeof(table));
    huffman_build_code_table(root, table, &error);

    print_code_table(table, freq);

    // 编码
    unsigned char encoded[256];
    size_t encoded_len = huffman_encode((const unsigned char*)text, len,
                                        encoded, sizeof(encoded), table, &error);

    if (error == HUFFMAN_OK) {
        printf("\n编码后长度: %zu 字节\n", encoded_len);
        printf("压缩率: %.1f%%\n", (1.0 - (double)encoded_len / len) * 100);

        // 解码
        unsigned char decoded[256];
        size_t decoded_len = huffman_decode(encoded, encoded_len,
                                            decoded, sizeof(decoded), root, &error);

        if (error == HUFFMAN_OK) {
            decoded[decoded_len] = '\0';
            printf("\n解码结果: %s\n", decoded);
            printf("解码长度: %zu 字节\n", decoded_len);

            if (strcmp((char*)decoded, text) == 0) {
                printf("\n✓ 编码解码成功，数据完整!\n");
            }
        }
    }

    huffman_free_tree(root);
}

// 演示 4: 压缩效果对比
static void demo_compression(void) {
    printf("\n=== 演示 4: 压缩效果对比 ===\n");

    struct {
        const char *text;
        const char *description;
    } cases[] = {
        {"aaaaabbbbbccccc", "高度重复文本"},
        {"abcdefghijklmno", "无重复文本"},
        {"the quick brown fox jumps over the lazy dog", "英文句子"},
        {NULL, NULL}
    };

    printf("不同文本的压缩效果:\n\n");

    for (int i = 0; cases[i].text; i++) {
        huffman_error_t error;
        size_t len = strlen(cases[i].text);

        printf("%s:\n", cases[i].description);
        printf("  原文: \"%s\"\n", cases[i].text);
        printf("  原文大小: %zu 字节\n", len);

        // 统计频率
        size_t freq[256] = {0};
        huffman_stats((const unsigned char*)cases[i].text, len, freq);

        // 创建树和编码表
        huffman_node_t *root = huffman_create_tree(freq, &error);
        if (root) {
            huffman_code_t table[256];
            memset(table, 0, sizeof(table));
            huffman_build_code_table(root, table, &error);

            // 计算编码后大小
            size_t bits = 0;
            for (size_t j = 0; j < len; j++) {
                bits += table[(unsigned char)cases[i].text[j]].code_len;
            }
            size_t bytes = (bits + 7) / 8;

            printf("  编码大小: %zu 字节 (%zu 位)\n", bytes, bits);
            printf("  压缩率: %.1f%%\n\n", (1.0 - (double)bytes / len) * 100);

            huffman_free_tree(root);
        }
    }
}

// 演示 5: 哈夫曼编码特点
static void demo_characteristics(void) {
    printf("\n=== 演示 5: 哈夫曼编码特点 ===\n");

    printf("1. 前缀编码\n");
    printf("   - 任何字符的编码都不是其他字符编码的前缀\n");
    printf("   - 保证唯一可解码性\n");
    printf("\n");

    printf("2. 最优性\n");
    printf("   - 对于给定频率分布，产生最短编码\n");
    printf("   - 最小化加权路径长度\n");
    printf("\n");

    printf("3. 变长编码\n");
    printf("   - 高频字符: 短编码\n");
    printf("   - 低频字符: 长编码\n");
    printf("\n");

    printf("4. 无损压缩\n");
    printf("   - 完全可逆\n");
    printf("   - 无信息丢失\n");
    printf("\n");

    printf("5. 需要传输编码表\n");
    printf("   - 编码表需要随数据一起传输\n");
    printf("   - 小文件可能反而变大\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 文件压缩\n");
    printf("   - ZIP, GZIP 使用哈夫曼编码\n");
    printf("   - 与 LZ77/LZ78 结合使用\n");
    printf("\n");

    printf("2. 图像压缩\n");
    printf("   - JPEG 使用哈夫曼编码\n");
    printf("   - 对量化后的系数编码\n");
    printf("\n");

    printf("3. 音频压缩\n");
    printf("   - MP3 使用哈夫曼编码\n");
    printf("   - 对频谱数据编码\n");
    printf("\n");

    printf("4. 数据传输\n");
    printf("   - 减少传输数据量\n");
    printf("   - 节省带宽\n");
    printf("\n");

    printf("5. 熵编码\n");
    printf("   - 信息论基础\n");
    printf("   - 接近熵极限的编码\n");
}

// 演示 7: 复杂度分析
static void demo_complexity(void) {
    printf("\n=== 演示 7: 复杂度分析 ===\n");

    printf("时间复杂度:\n");
    printf("  频率统计:   O(n)\n");
    printf("  建堆:       O(k log k)  k=不同字符数\n");
    printf("  构建编码表: O(k)\n");
    printf("  编码:       O(n)\n");
    printf("  解码:       O(n)\n");
    printf("\n");

    printf("空间复杂度:\n");
    printf("  O(k) - 存储哈夫曼树和编码表\n");
    printf("\n");

    printf("注意:\n");
    printf("  n = 输入数据长度\n");
    printf("  k = 不同字符数量 (k <= 256)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    哈夫曼编码演示\n");
    printf("========================================\n");

    demo_frequency();
    demo_build_tree();
    demo_encode_decode();
    demo_compression();
    demo_characteristics();
    demo_applications();
    demo_complexity();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
