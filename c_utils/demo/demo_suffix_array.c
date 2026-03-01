#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/suffix_array.h"

void print_suffix_array(const char *s, const int *sa, size_t n) {
    printf("   后缀数组:\n");
    for (size_t i = 0; i < n; i++) {
        printf("   %2zu: %2d -> %s\n", i, sa[i], s + sa[i]);
    }
}

int main() {
    printf("=== Suffix Array Demo ===\n\n");
    
    // 测试字符串
    const char *test_str = "banana";
    size_t n = strlen(test_str);
    
    printf("测试字符串: %s\n", test_str);
    printf("字符串长度: %zu\n\n", n);
    
    // 测试基本后缀数组构建
    printf("1. 基本后缀数组构建:\n");
    int *sa = (int *)malloc(n * sizeof(int));
    if (!sa) {
        printf("   内存分配失败\n");
        return 1;
    }
    
    suffix_array_build(test_str, sa);
    print_suffix_array(test_str, sa, n);
    
    // 测试不同的字符串
    printf("\n2. 测试不同的字符串:\n");
    
    // 测试空字符串
    const char *empty_str = "";
    size_t empty_n = strlen(empty_str);
    int *empty_sa = (int *)malloc(empty_n * sizeof(int));
    if (empty_sa) {
        suffix_array_build(empty_str, empty_sa);
        printf("   空字符串: ");
        if (empty_n == 0) {
            printf("无后缀数组\n");
        } else {
            print_suffix_array(empty_str, empty_sa, empty_n);
        }
        free(empty_sa);
    }
    
    // 测试单字符字符串
    const char *single_str = "a";
    size_t single_n = strlen(single_str);
    int *single_sa = (int *)malloc(single_n * sizeof(int));
    if (single_sa) {
        suffix_array_build(single_str, single_sa);
        printf("   单字符字符串: %s\n", single_str);
        print_suffix_array(single_str, single_sa, single_n);
        free(single_sa);
    }
    
    // 测试重复字符字符串
    const char *repeat_str = "aaaaa";
    size_t repeat_n = strlen(repeat_str);
    int *repeat_sa = (int *)malloc(repeat_n * sizeof(int));
    if (repeat_sa) {
        suffix_array_build(repeat_str, repeat_sa);
        printf("   重复字符字符串: %s\n", repeat_str);
        print_suffix_array(repeat_str, repeat_sa, repeat_n);
        free(repeat_sa);
    }
    
    // 测试不同字符字符串
    const char *unique_str = "abcde";
    size_t unique_n = strlen(unique_str);
    int *unique_sa = (int *)malloc(unique_n * sizeof(int));
    if (unique_sa) {
        suffix_array_build(unique_str, unique_sa);
        printf("   不同字符字符串: %s\n", unique_str);
        print_suffix_array(unique_str, unique_sa, unique_n);
        free(unique_sa);
    }
    
    // 释放资源
    free(sa);
    
    printf("\n=== Suffix Array Demo 完成 ===\n");
    return 0;
}