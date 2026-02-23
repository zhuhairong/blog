#ifndef C_UTILS_HUFFMAN_H
#define C_UTILS_HUFFMAN_H

#include <stddef.h>
#include <stdbool.h>

// 哈夫曼错误码
typedef enum {
    HUFFMAN_OK = 0,
    HUFFMAN_INVALID_INPUT = -1,
    HUFFMAN_MEMORY_ERROR = -2,
    HUFFMAN_TREE_ERROR = -3,
    HUFFMAN_ENCODE_ERROR = -4,
    HUFFMAN_DECODE_ERROR = -5,
    HUFFMAN_BUFFER_TOO_SMALL = -6
} huffman_error_t;

// 哈夫曼节点
typedef struct huffman_node {
    unsigned char symbol;
    size_t freq;
    struct huffman_node *left;
    struct huffman_node *right;
} huffman_node_t;

// 哈夫曼编码表项
typedef struct {
    unsigned char symbol;
    unsigned int code;
    size_t code_len;
} huffman_code_t;

// 哈夫曼配置
typedef struct {
    bool enable_stats;
    size_t max_tree_depth;
} huffman_config_t;

// 哈夫曼频率统计
void huffman_stats(const unsigned char *data, size_t len, size_t freq[256]);

// 创建哈夫曼树
huffman_node_t *huffman_create_tree(const size_t freq[256], huffman_error_t *error);

// 构建哈夫曼编码表
int huffman_build_code_table(huffman_node_t *root, huffman_code_t table[256], huffman_error_t *error);

// 哈夫曼编码
size_t huffman_encode(const unsigned char *input, size_t input_len, unsigned char *output, size_t output_size, 
                      const huffman_code_t table[256], huffman_error_t *error);

// 哈夫曼解码
size_t huffman_decode(const unsigned char *input, size_t input_len, unsigned char *output, size_t output_size, 
                      huffman_node_t *root, huffman_error_t *error);

// 释放哈夫曼树
void huffman_free_tree(huffman_node_t *root);

// 获取默认哈夫曼配置
void huffman_get_default_config(huffman_config_t *config);

#endif // C_UTILS_HUFFMAN_H
