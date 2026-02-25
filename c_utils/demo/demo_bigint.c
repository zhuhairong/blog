/**
 * @file demo_bigint.c
 * @brief 大整数运算演示
 *
 * 本演示展示了大整数的各种运算，包括：
 * - 创建和销毁
 * - 基本运算（加、减、乘）
 * - 比较操作
 * - 字符串转换
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bigint.h"

// 打印大整数
static void print_bigint(const char* label, const bigint_t* b) {
    char* str = bigint_to_str(b);
    if (str) {
        printf("%s%s\n", label, str);
        free(str);
    }
}

// 演示 1: 从字符串创建大整数
static void demo_from_string(void) {
    printf("\n=== 演示 1: 从字符串创建大整数 ===\n");

    const char* numbers[] = {
        "0",
        "12345678901234567890",
        "999999999999999999999999999999",
        "1234567890123456789012345678901234567890"
    };
    int num_count = sizeof(numbers) / sizeof(numbers[0]);

    for (int i = 0; i < num_count; i++) {
        printf("输入: \"%s\"\n", numbers[i]);

        bigint_t* b = bigint_from_str(numbers[i]);
        if (b) {
            print_bigint("大整数: ", b);
            printf("位数: %zu\n\n", b->len);
            bigint_free(b);
        } else {
            printf("创建失败!\n\n");
        }
    }
}

// 演示 2: 基本常量
static void demo_constants(void) {
    printf("\n=== 演示 2: 基本常量 ===\n");

    bigint_t* zero = bigint_zero();
    bigint_t* one = bigint_one();

    print_bigint("零: ", zero);
    print_bigint("一: ", one);

    printf("\n零的位数: %zu\n", zero->len);
    printf("一的位数: %zu\n", one->len);

    bigint_free(zero);
    bigint_free(one);
}

// 演示 3: 加法运算
static void demo_addition(void) {
    printf("\n=== 演示 3: 加法运算 ===\n");

    struct {
        const char* a;
        const char* b;
    } test_cases[] = {
        {"123", "456"},
        {"999", "1"},
        {"99999999999999999999", "1"},
        {"12345678901234567890", "98765432109876543210"},
        {"999999999999999999999999999999", "1"}
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        bigint_t* a = bigint_from_str(test_cases[i].a);
        bigint_t* b = bigint_from_str(test_cases[i].b);

        if (a && b) {
            bigint_t* sum = bigint_add(a, b);

            printf("%s + %s = ", test_cases[i].a, test_cases[i].b);
            print_bigint("", sum);

            bigint_free(sum);
        }

        bigint_free(a);
        bigint_free(b);
    }
}

// 演示 4: 减法运算
static void demo_subtraction(void) {
    printf("\n=== 演示 4: 减法运算 ===\n");

    struct {
        const char* a;
        const char* b;
    } test_cases[] = {
        {"456", "123"},
        {"1000", "1"},
        {"100000000000000000000", "1"},
        {"12345678901234567890", "12345678901234567890"},
        {"99999999999999999999", "88888888888888888888"}
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        bigint_t* a = bigint_from_str(test_cases[i].a);
        bigint_t* b = bigint_from_str(test_cases[i].b);

        if (a && b) {
            bigint_t* diff = bigint_sub(a, b);

            printf("%s - %s = ", test_cases[i].a, test_cases[i].b);
            print_bigint("", diff);

            bigint_free(diff);
        }

        bigint_free(a);
        bigint_free(b);
    }
}

// 演示 5: 乘法运算
static void demo_multiplication(void) {
    printf("\n=== 演示 5: 乘法运算 ===\n");

    struct {
        const char* a;
        const char* b;
    } test_cases[] = {
        {"12", "34"},
        {"123", "456"},
        {"999", "999"},
        {"123456789", "987654321"},
        {"1000000000", "1000000000"}
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        bigint_t* a = bigint_from_str(test_cases[i].a);
        bigint_t* b = bigint_from_str(test_cases[i].b);

        if (a && b) {
            bigint_t* prod = bigint_mul(a, b);

            printf("%s * %s = ", test_cases[i].a, test_cases[i].b);
            print_bigint("", prod);

            bigint_free(prod);
        }

        bigint_free(a);
        bigint_free(b);
    }
}

// 演示 6: 比较运算
static void demo_comparison(void) {
    printf("\n=== 演示 6: 比较运算 ===\n");

    struct {
        const char* a;
        const char* b;
    } test_cases[] = {
        {"123", "456"},
        {"456", "123"},
        {"123", "123"},
        {"99999999999999999999", "100000000000000000000"},
        {"12345678901234567890", "12345678901234567890"}
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        bigint_t* a = bigint_from_str(test_cases[i].a);
        bigint_t* b = bigint_from_str(test_cases[i].b);

        if (a && b) {
            int cmp = bigint_compare(a, b);
            const char* result;
            if (cmp < 0) result = "<";
            else if (cmp > 0) result = ">";
            else result = "==";

            printf("%s %s %s\n", test_cases[i].a, result, test_cases[i].b);
        }

        bigint_free(a);
        bigint_free(b);
    }
}

// 演示 7: 复制操作
static void demo_copy(void) {
    printf("\n=== 演示 7: 复制操作 ===\n");

    bigint_t* original = bigint_from_str("123456789012345678901234567890");
    if (original) {
        print_bigint("原始值: ", original);

        bigint_t* copy = bigint_copy(original);
        if (copy) {
            print_bigint("复制值: ", copy);

            // 验证是深拷贝
            printf("\n验证深拷贝:\n");
            printf("  原始值地址: %p\n", (void*)original->digits);
            printf("  复制值地址: %p\n", (void*)copy->digits);
            printf("  地址不同: %s\n",
                   original->digits != copy->digits ? "是" : "否");

            bigint_free(copy);
        }

        bigint_free(original);
    }
}

// 演示 8: 大数运算
static void demo_large_numbers(void) {
    printf("\n=== 演示 8: 大数运算 ===\n");

    // 计算阶乘类的大数
    bigint_t* result = bigint_one();
    if (!result) return;

    printf("计算大数乘法链:\n");
    printf("1");

    for (int i = 2; i <= 20; i++) {
        char num_str[4];
        snprintf(num_str, sizeof(num_str), "%d", i);

        bigint_t* multiplier = bigint_from_str(num_str);
        if (multiplier) {
            bigint_t* new_result = bigint_mul(result, multiplier);
            bigint_free(result);
            result = new_result;

            printf(" * %d", i);
        }
        bigint_free(multiplier);
    }

    printf(" =\n");
    print_bigint("", result);

    bigint_free(result);
}

// 演示 9: 零值判断
static void demo_zero_check(void) {
    printf("\n=== 演示 9: 零值判断 ===\n");

    bigint_t* zero = bigint_zero();
    bigint_t* one = bigint_one();
    bigint_t* large = bigint_from_str("12345678901234567890");

    if (zero && one && large) {
        printf("零值判断测试:\n");
        printf("  bigint_zero(): %s\n",
               bigint_is_zero(zero) ? "是零" : "非零");
        printf("  bigint_one(): %s\n",
               bigint_is_zero(one) ? "是零" : "非零");
        printf("  大数: %s\n",
               bigint_is_zero(large) ? "是零" : "非零");
    }

    bigint_free(zero);
    bigint_free(one);
    bigint_free(large);
}

// 演示 10: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 10: 大数运算性能 ===\n");

    // 创建两个大数
    bigint_t* a = bigint_from_str("1234567890123456789012345678901234567890");
    bigint_t* b = bigint_from_str("9876543210987654321098765432109876543210");

    if (a && b) {
        printf("大数 A: ");
        print_bigint("", a);
        printf("大数 B: ");
        print_bigint("", b);

        printf("\n运算结果:\n");

        bigint_t* sum = bigint_add(a, b);
        printf("A + B = ");
        print_bigint("", sum);
        bigint_free(sum);

        bigint_t* diff = bigint_sub(b, a);
        printf("B - A = ");
        print_bigint("", diff);
        bigint_free(diff);

        bigint_t* prod = bigint_mul(a, b);
        printf("A * B = ");
        print_bigint("", prod);
        bigint_free(prod);
    }

    bigint_free(a);
    bigint_free(b);
}

int main(void) {
    printf("========================================\n");
    printf("    大整数运算演示程序\n");
    printf("========================================\n");

    demo_from_string();
    demo_constants();
    demo_addition();
    demo_subtraction();
    demo_multiplication();
    demo_comparison();
    demo_copy();
    demo_large_numbers();
    demo_zero_check();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
