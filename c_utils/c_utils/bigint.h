#ifndef C_UTILS_BIGINT_H
#define C_UTILS_BIGINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// 大整数实现（支持正整数）
typedef struct {
    uint32_t *digits;
    size_t len;
} bigint_t;

// 创建与销毁
bigint_t* bigint_from_str(const char *str);
void      bigint_free(bigint_t *b);
char*     bigint_to_str(const bigint_t *b);
bigint_t* bigint_copy(const bigint_t *b);

// 常量创建
bigint_t* bigint_zero(void);
bigint_t* bigint_one(void);

// 基本运算
bigint_t* bigint_add(const bigint_t *a, const bigint_t *b);
bigint_t* bigint_sub(const bigint_t *a, const bigint_t *b);  // a >= b
bigint_t* bigint_mul(const bigint_t *a, const bigint_t *b);

// 比较与判断
int       bigint_compare(const bigint_t *a, const bigint_t *b); // -1, 0, 1
bool      bigint_is_zero(const bigint_t *b);

#endif // C_UTILS_BIGINT_H
