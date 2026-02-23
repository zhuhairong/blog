#include "bigint.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// 内部辅助函数：创建指定长度的 bigint
static bigint_t* bigint_create(size_t len) {
    bigint_t *b = malloc(sizeof(bigint_t));
    if (!b) return NULL;
    b->len = len;
    b->digits = calloc(len, sizeof(uint32_t));
    if (!b->digits) {
        free(b);
        return NULL;
    }
    return b;
}

// 内部辅助函数：去除前导零
static void bigint_trim(bigint_t *b) {
    if (!b || b->len == 0) return;
    while (b->len > 1 && b->digits[b->len - 1] == 0) {
        b->len--;
    }
}

bigint_t* bigint_from_str(const char *str) {
    if (!str || *str == '\0') return NULL;
    
    // 跳过前导空格
    while (isspace((unsigned char)*str)) str++;
    
    // 检查是否为空（全是空格）
    if (*str == '\0') return NULL;
    
    // 检查负号（暂不支持负数）
    if (*str == '-') return NULL;
    
    // 跳过前导零
    while (*str == '0' && *(str + 1) != '\0') str++;
    
    size_t n = strlen(str);
    // 检查是否全为数字
    for (size_t i = 0; i < n; i++) {
        if (!isdigit((unsigned char)str[i])) return NULL;
    }
    
    bigint_t *b = bigint_create(n);
    if (!b) return NULL;
    
    for (size_t i = 0; i < n; i++) {
        b->digits[i] = str[n - 1 - i] - '0';
    }
    
    return b;
}

void bigint_free(bigint_t *b) {
    if (!b) return;
    free(b->digits);
    free(b);
}

char* bigint_to_str(const bigint_t *b) {
    if (!b || b->len == 0) return NULL;
    
    char *str = malloc(b->len + 1);
    if (!str) return NULL;
    
    for (size_t i = 0; i < b->len; i++) {
        str[i] = b->digits[b->len - 1 - i] + '0';
    }
    str[b->len] = '\0';
    return str;
}

bigint_t* bigint_add(const bigint_t *a, const bigint_t *b) {
    if (!a || !b) return NULL;
    
    size_t max_len = (a->len > b->len ? a->len : b->len) + 1;
    bigint_t *res = bigint_create(max_len);
    if (!res) return NULL;
    
    uint32_t carry = 0;
    for (size_t i = 0; i < max_len; i++) {
        uint32_t val = carry;
        if (i < a->len) val += a->digits[i];
        if (i < b->len) val += b->digits[i];
        res->digits[i] = val % 10;
        carry = val / 10;
    }
    
    bigint_trim(res);
    return res;
}

// 新增：创建零值
bigint_t* bigint_zero(void) {
    return bigint_from_str("0");
}

// 新增：创建一值
bigint_t* bigint_one(void) {
    return bigint_from_str("1");
}

// 新增：复制
bigint_t* bigint_copy(const bigint_t *b) {
    if (!b) return NULL;
    
    bigint_t *copy = bigint_create(b->len);
    if (!copy) return NULL;
    
    memcpy(copy->digits, b->digits, b->len * sizeof(uint32_t));
    return copy;
}

// 新增：比较 (-1: a<b, 0: a==b, 1: a>b)
int bigint_compare(const bigint_t *a, const bigint_t *b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    if (a->len != b->len) {
        return (a->len > b->len) ? 1 : -1;
    }
    
    for (int i = (int)a->len - 1; i >= 0; i--) {
        if (a->digits[i] != b->digits[i]) {
            return (a->digits[i] > b->digits[i]) ? 1 : -1;
        }
    }
    
    return 0;
}

// 新增：是否为零
bool bigint_is_zero(const bigint_t *b) {
    if (!b || b->len == 0) return true;
    return (b->len == 1 && b->digits[0] == 0);
}

// 新增：减法 (假设 a >= b)
bigint_t* bigint_sub(const bigint_t *a, const bigint_t *b) {
    if (!a || !b) return NULL;
    if (bigint_compare(a, b) < 0) return NULL; // 暂不支持负数结果
    
    bigint_t *res = bigint_create(a->len);
    if (!res) return NULL;
    
    int32_t borrow = 0;
    for (size_t i = 0; i < a->len; i++) {
        int32_t val = (int32_t)a->digits[i] - borrow;
        if (i < b->len) val -= b->digits[i];
        
        if (val < 0) {
            val += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        res->digits[i] = (uint32_t)val;
    }
    
    bigint_trim(res);
    return res;
}

// 新增：乘法
bigint_t* bigint_mul(const bigint_t *a, const bigint_t *b) {
    if (!a || !b) return NULL;
    
    // 特殊情况：乘以零
    if (bigint_is_zero(a) || bigint_is_zero(b)) {
        return bigint_zero();
    }
    
    size_t res_len = a->len + b->len;
    bigint_t *res = bigint_create(res_len);
    if (!res) return NULL;
    
    for (size_t i = 0; i < a->len; i++) {
        uint32_t carry = 0;
        for (size_t j = 0; j < b->len || carry; j++) {
            uint64_t val = res->digits[i + j] + (uint64_t)a->digits[i] * 
                          (j < b->len ? b->digits[j] : 0) + carry;
            res->digits[i + j] = (uint32_t)(val % 10);
            carry = (uint32_t)(val / 10);
        }
    }
    
    bigint_trim(res);
    return res;
}
