#ifndef C_UTILS_MATRIX_H
#define C_UTILS_MATRIX_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} matrix_t;

// 创建与销毁
matrix_t* matrix_create(size_t rows, size_t cols);
void      matrix_free(matrix_t *m);
matrix_t* matrix_copy(const matrix_t *m);

// 基本操作
void      matrix_set(matrix_t *m, size_t r, size_t c, double val);
double    matrix_get(const matrix_t *m, size_t r, size_t c);
void      matrix_fill(matrix_t *m, double val);
void      matrix_zero(matrix_t *m);

// 矩阵运算
matrix_t* matrix_add(const matrix_t *a, const matrix_t *b);
matrix_t* matrix_sub(const matrix_t *a, const matrix_t *b);
matrix_t* matrix_mul(const matrix_t *a, const matrix_t *b);
matrix_t* matrix_transpose(const matrix_t *m);
matrix_t* matrix_scalar_mul(const matrix_t *m, double scalar);

// 特殊矩阵
matrix_t* matrix_identity(size_t n);

// 比较与判断
bool      matrix_equal(const matrix_t *a, const matrix_t *b, double epsilon);
bool      matrix_is_square(const matrix_t *m);
double    matrix_trace(const matrix_t *m);

#endif // C_UTILS_MATRIX_H
