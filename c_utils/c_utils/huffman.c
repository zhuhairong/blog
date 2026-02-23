#include "huffman.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void huffman_stats(const unsigned char *data, size_t len, size_t freq[256]) {
    if (freq) {
        memset(freq, 0, 256 * sizeof(size_t));
        if (data && len > 0) {
            for (size_t i = 0; i < len; i++) freq[data[i]]++;
        }
    }
}

static huffman_node_t *create_node(unsigned char symbol, size_t freq) {
    huffman_node_t *node = (huffman_node_t *)malloc(sizeof(huffman_node_t));
    if (node) {
        node->symbol = symbol;
        node->freq = freq;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

static int compare_nodes(const void *a, const void *b) {
    const huffman_node_t *node1 = *(const huffman_node_t **)a;
    const huffman_node_t *node2 = *(const huffman_node_t **)b;
    return (int)(node1->freq - node2->freq);
}

huffman_node_t *huffman_create_tree(const size_t freq[256], huffman_error_t *error) {
    if (!freq) {
        if (error) *error = HUFFMAN_INVALID_INPUT;
        return NULL;
    }

    huffman_node_t *nodes[256];
    size_t node_count = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            nodes[node_count] = create_node((unsigned char)i, freq[i]);
            if (!nodes[node_count]) {
                for (size_t j = 0; j < node_count; j++) {
                    free(nodes[j]);
                }
                if (error) *error = HUFFMAN_MEMORY_ERROR;
                return NULL;
            }
            node_count++;
        }
    }

    if (node_count == 0) {
        if (error) *error = HUFFMAN_TREE_ERROR;
        return NULL;
    }

    while (node_count > 1) {
        qsort(nodes, node_count, sizeof(huffman_node_t *), compare_nodes);

        huffman_node_t *parent = create_node(0, nodes[0]->freq + nodes[1]->freq);
        if (!parent) {
            for (size_t i = 0; i < node_count; i++) {
                free(nodes[i]);
            }
            if (error) *error = HUFFMAN_MEMORY_ERROR;
            return NULL;
        }

        parent->left = nodes[0];
        parent->right = nodes[1];

        nodes[0] = parent;
        for (size_t i = 1; i < node_count - 1; i++) {
            nodes[i] = nodes[i + 1];
        }
        node_count--;
    }

    if (error) *error = HUFFMAN_OK;
    return nodes[0];
}

static void build_code_table_recursive(huffman_node_t *node, huffman_code_t table[256], 
                                       unsigned int code, size_t code_len) {
    if (!node) return;

    if (!node->left && !node->right) {
        table[node->symbol].symbol = node->symbol;
        table[node->symbol].code = code;
        table[node->symbol].code_len = code_len;
        return;
    }

    build_code_table_recursive(node->left, table, code << 1, code_len + 1);
    build_code_table_recursive(node->right, table, (code << 1) | 1, code_len + 1);
}

int huffman_build_code_table(huffman_node_t *root, huffman_code_t table[256], huffman_error_t *error) {
    if (!root || !table) {
        if (error) *error = HUFFMAN_INVALID_INPUT;
        return -1;
    }

    for (int i = 0; i < 256; i++) {
        table[i].symbol = (unsigned char)i;
        table[i].code = 0;
        table[i].code_len = 0;
    }

    build_code_table_recursive(root, table, 0, 0);

    if (error) *error = HUFFMAN_OK;
    return 0;
}

size_t huffman_encode(const unsigned char *input, size_t input_len, unsigned char *output, size_t output_size, 
                      const huffman_code_t table[256], huffman_error_t *error) {
    if (!input || !output || !table || input_len == 0) {
        if (error) *error = HUFFMAN_INVALID_INPUT;
        return 0;
    }

    size_t bit_pos = 0;
    size_t byte_pos = 0;

    for (size_t i = 0; i < input_len; i++) {
        unsigned char symbol = input[i];
        unsigned int code = table[symbol].code;
        size_t code_len = table[symbol].code_len;

        if (code_len == 0) {
            if (error) *error = HUFFMAN_ENCODE_ERROR;
            return 0;
        }

        for (size_t j = 0; j < code_len; j++) {
            if (byte_pos >= output_size) {
                if (error) *error = HUFFMAN_BUFFER_TOO_SMALL;
                return 0;
            }

            unsigned int bit = (code >> (code_len - j - 1)) & 1;
            output[byte_pos] |= (bit << (7 - (bit_pos % 8)));
            bit_pos++;

            if (bit_pos % 8 == 0) {
                byte_pos++;
            }
        }
    }

    if (bit_pos % 8 != 0) {
        byte_pos++;
    }

    if (error) *error = HUFFMAN_OK;
    return byte_pos;
}

size_t huffman_decode(const unsigned char *input, size_t input_len, unsigned char *output, size_t output_size, 
                      huffman_node_t *root, huffman_error_t *error) {
    if (!input || !output || !root || input_len == 0) {
        if (error) *error = HUFFMAN_INVALID_INPUT;
        return 0;
    }

    size_t bit_pos = 0;
    size_t byte_pos = 0;
    huffman_node_t *current = root;

    while (bit_pos < input_len * 8 && byte_pos < output_size) {
        if (!current->left && !current->right) {
            output[byte_pos++] = current->symbol;
            current = root;
        }

        unsigned int bit = (input[bit_pos / 8] >> (7 - (bit_pos % 8))) & 1;
        bit_pos++;

        if (bit == 0) {
            if (!current->left) {
                if (error) *error = HUFFMAN_DECODE_ERROR;
                return 0;
            }
            current = current->left;
        } else {
            if (!current->right) {
                if (error) *error = HUFFMAN_DECODE_ERROR;
                return 0;
            }
            current = current->right;
        }
    }

    if (!current->left && !current->right && byte_pos < output_size) {
        output[byte_pos++] = current->symbol;
    }

    if (error) *error = HUFFMAN_OK;
    return byte_pos;
}

void huffman_free_tree(huffman_node_t *root) {
    if (root) {
        huffman_free_tree(root->left);
        huffman_free_tree(root->right);
        free(root);
    }
}

void huffman_get_default_config(huffman_config_t *config) {
    if (config) {
        config->enable_stats = true;
        config->max_tree_depth = 32;
    }
}
