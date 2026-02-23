#include "matrix.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

matrix_t* matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) return NULL;
    
    matrix_t *m = malloc(sizeof(matrix_t));
    if (!m) return NULL;
    
    m->rows = rows;
    m->cols = cols;
    m->data = calloc(rows * cols, sizeof(double));
    
    if (!m->data) {
        free(m);
        return NULL;
    }
    
    return m;
}

void matrix_free(matrix_t *m) {
    if (!m) return;
    free(m->data);
    free(m);
}

void matrix_set(matrix_t *m, size_t r, size_t c, double val) {
    if (!m || r >= m->rows || c >= m->cols) return;
    m->data[r * m->cols + c] = val;
}

double matrix_get(const matrix_t *m, size_t r, size_t c) {
    if (!m || r >= m->rows || c >= m->cols) return 0.0;
    return m->data[r * m->cols + c];
}

matrix_t* matrix_copy(const matrix_t *m) {
    if (!m) return NULL;
    
    matrix_t *copy = matrix_create(m->rows, m->cols);
    if (!copy) return NULL;
    
    memcpy(copy->data, m->data, m->rows * m->cols * sizeof(double));
    return copy;
}

matrix_t* matrix_add(const matrix_t *a, const matrix_t *b) {
    if (!a || !b) return NULL;
    if (a->rows != b->rows || a->cols != b->cols) return NULL;
    
    matrix_t *res = matrix_create(a->rows, a->cols);
    if (!res) return NULL;
    
    for (size_t i = 0; i < a->rows * a->cols; i++) {
        res->data[i] = a->data[i] + b->data[i];
    }
    
    return res;
}

matrix_t* matrix_sub(const matrix_t *a, const matrix_t *b) {
    if (!a || !b) return NULL;
    if (a->rows != b->rows || a->cols != b->cols) return NULL;
    
    matrix_t *res = matrix_create(a->rows, a->cols);
    if (!res) return NULL;
    
    for (size_t i = 0; i < a->rows * a->cols; i++) {
        res->data[i] = a->data[i] - b->data[i];
    }
    
    return res;
}

matrix_t* matrix_mul(const matrix_t *a, const matrix_t *b) {
    if (!a || !b) return NULL;
    if (a->cols != b->rows) return NULL;
    
    matrix_t *res = matrix_create(a->rows, b->cols);
    if (!res) return NULL;
    
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0;
            for (size_t k = 0; k < a->cols; k++) {
                sum += matrix_get(a, i, k) * matrix_get(b, k, j);
            }
            matrix_set(res, i, j, sum);
        }
    }
    
    return res;
}

matrix_t* matrix_transpose(const matrix_t *m) {
    if (!m) return NULL;
    
    matrix_t *res = matrix_create(m->cols, m->rows);
    if (!res) return NULL;
    
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++) {
            matrix_set(res, j, i, matrix_get(m, i, j));
        }
    }
    
    return res;
}

matrix_t* matrix_identity(size_t n) {
    if (n == 0) return NULL;
    
    matrix_t *m = matrix_create(n, n);
    if (!m) return NULL;
    
    for (size_t i = 0; i < n; i++) {
        matrix_set(m, i, i, 1.0);
    }
    
    return m;
}

matrix_t* matrix_scalar_mul(const matrix_t *m, double scalar) {
    if (!m) return NULL;
    
    matrix_t *res = matrix_create(m->rows, m->cols);
    if (!res) return NULL;
    
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        res->data[i] = m->data[i] * scalar;
    }
    
    return res;
}

bool matrix_equal(const matrix_t *a, const matrix_t *b, double epsilon) {
    if (!a || !b) return false;
    if (a->rows != b->rows || a->cols != b->cols) return false;
    
    for (size_t i = 0; i < a->rows * a->cols; i++) {
        if (fabs(a->data[i] - b->data[i]) > epsilon) {
            return false;
        }
    }
    
    return true;
}

bool matrix_is_square(const matrix_t *m) {
    return m && m->rows == m->cols;
}

double matrix_trace(const matrix_t *m) {
    if (!matrix_is_square(m)) return 0.0;
    
    double trace = 0.0;
    for (size_t i = 0; i < m->rows; i++) {
        trace += matrix_get(m, i, i);
    }
    
    return trace;
}

void matrix_fill(matrix_t *m, double val) {
    if (!m) return;
    
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        m->data[i] = val;
    }
}

void matrix_zero(matrix_t *m) {
    if (!m) return;
    memset(m->data, 0, m->rows * m->cols * sizeof(double));
}
