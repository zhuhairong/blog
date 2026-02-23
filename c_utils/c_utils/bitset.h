#ifndef C_UTILS_BITSET_H
#define C_UTILS_BITSET_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct bitset_s bitset_t;

// 创建与销毁
bitset_t* bitset_create(size_t nbits);
bitset_t* bitset_create_from_bytes(const uint8_t *bytes, size_t nbytes, size_t nbits);
bitset_t* bitset_clone(const bitset_t *bs);
void      bitset_free(bitset_t *bs);

// 基本操作
void      bitset_set(bitset_t *bs, size_t bit);
void      bitset_clear(bitset_t *bs, size_t bit);
bool      bitset_test(const bitset_t *bs, size_t bit);
void      bitset_flip(bitset_t *bs, size_t bit);

// 批量操作
void      bitset_set_all(bitset_t *bs);
void      bitset_clear_all(bitset_t *bs);
void      bitset_flip_all(bitset_t *bs);
void      bitset_set_range(bitset_t *bs, size_t start, size_t end);
void      bitset_clear_range(bitset_t *bs, size_t start, size_t end);
void      bitset_flip_range(bitset_t *bs, size_t start, size_t end);

// 位运算
bitset_t* bitset_and(const bitset_t *a, const bitset_t *b);
bitset_t* bitset_or(const bitset_t *a, const bitset_t *b);
bitset_t* bitset_xor(const bitset_t *a, const bitset_t *b);
bitset_t* bitset_not(const bitset_t *bs);

// 计数与查找
size_t    bitset_count(const bitset_t *bs);
size_t    bitset_count_range(const bitset_t *bs, size_t start, size_t end);
size_t    bitset_find_first_set(const bitset_t *bs, size_t start);
size_t    bitset_find_first_clear(const bitset_t *bs, size_t start);
size_t    bitset_find_last_set(const bitset_t *bs);

// 状态查询
bool      bitset_is_empty(const bitset_t *bs);
bool      bitset_is_all_set(const bitset_t *bs);
bool      bitset_equals(const bitset_t *a, const bitset_t *b);
size_t    bitset_size(const bitset_t *bs);
size_t    bitset_byte_size(const bitset_t *bs);

// 调整大小
bool      bitset_resize(bitset_t *bs, size_t new_size);

// 序列化
bool      bitset_to_bytes(const bitset_t *bs, uint8_t *out, size_t out_size);

#endif // C_UTILS_BITSET_H
