#ifndef C_UTILS_BITSET_COMPRESSED_H
#define C_UTILS_BITSET_COMPRESSED_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// RLE 压缩位图
typedef struct {
    uint32_t *runs;  // 交替存储：[长度, 值, 长度, 值, ...]
    size_t count;     // 运行数量（每个运行占用2个uint32_t）
    size_t capacity;  // 容量（uint32_t数量）
    size_t total_bits; // 总位数
} bitset_compressed_t;

// 创建与销毁
void bitset_comp_init(bitset_compressed_t *bc);
bitset_compressed_t* bitset_comp_create(size_t estimated_size);
bitset_compressed_t* bitset_comp_clone(const bitset_compressed_t *bc);
void bitset_comp_free(bitset_compressed_t *bc);

// 添加运行
void bitset_comp_add_run(bitset_compressed_t *bc, uint32_t start, uint32_t len);
bool bitset_comp_add_bit(bitset_compressed_t *bc, size_t bit, bool value);

// 位操作
bool bitset_comp_test(const bitset_compressed_t *bc, size_t bit);
bool bitset_comp_set(bitset_compressed_t *bc, size_t bit);
bool bitset_comp_clear(bitset_compressed_t *bc, size_t bit);
bool bitset_comp_flip(bitset_compressed_t *bc, size_t bit);

// 批量操作
bool bitset_comp_set_range(bitset_compressed_t *bc, size_t start, size_t end, bool value);

// 计数与统计
size_t bitset_comp_count(const bitset_compressed_t *bc, bool value);
size_t bitset_comp_count_range(const bitset_compressed_t *bc, size_t start, size_t end, bool value);
size_t bitset_comp_find_first(const bitset_compressed_t *bc, bool value, size_t start);
size_t bitset_comp_find_last(const bitset_compressed_t *bc, bool value);

// 状态查询
bool bitset_comp_is_empty(const bitset_compressed_t *bc);
size_t bitset_comp_size(const bitset_compressed_t *bc);
size_t bitset_comp_run_count(const bitset_compressed_t *bc);
size_t bitset_comp_byte_size(const bitset_compressed_t *bc);

// 序列化与反序列化
bool bitset_comp_to_bytes(const bitset_compressed_t *bc, uint8_t *out, size_t out_size);
bitset_compressed_t* bitset_comp_from_bytes(const uint8_t *in, size_t in_size);

// 比较与操作
bool bitset_comp_equals(const bitset_compressed_t *a, const bitset_compressed_t *b);
bitset_compressed_t* bitset_comp_and(const bitset_compressed_t *a, const bitset_compressed_t *b);
bitset_compressed_t* bitset_comp_or(const bitset_compressed_t *a, const bitset_compressed_t *b);
bitset_compressed_t* bitset_comp_xor(const bitset_compressed_t *a, const bitset_compressed_t *b);
bitset_compressed_t* bitset_comp_not(const bitset_compressed_t *bc);

#endif // C_UTILS_BITSET_COMPRESSED_H
