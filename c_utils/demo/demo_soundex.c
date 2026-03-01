#include <stdio.h>
#include "../c_utils/soundex.h"

int main() {
    printf("=== Soundex Demo ===\n\n");
    
    // 测试基本 Soundex 编码
    printf("1. 基本 Soundex 编码:\n");
    char soundex_code[SOUNDEX_LENGTH + 1];
    
    const char* test_names[] = {"Smith", "Smyth", "Johnson", "Johnston", "Williams", "Wilson"};
    size_t num_names = sizeof(test_names) / sizeof(test_names[0]);
    
    for (size_t i = 0; i < num_names; i++) {
        soundex_encode(test_names[i], soundex_code);
        printf("   %-10s -> %s\n", test_names[i], soundex_code);
    }
    
    // 测试不同拼写但发音相似的单词
    printf("\n2. 发音相似单词的 Soundex 编码:\n");
    const char* similar_words[] = {"Knight", "Night", "Cnight", "Nite", "Knite"};
    size_t num_similar = sizeof(similar_words) / sizeof(similar_words[0]);
    
    for (size_t i = 0; i < num_similar; i++) {
        soundex_encode(similar_words[i], soundex_code);
        printf("   %-10s -> %s\n", similar_words[i], soundex_code);
    }
    
    // 测试空字符串和单字符字符串
    printf("\n3. 特殊情况测试:\n");
    soundex_encode("", soundex_code);
    printf("   空字符串 -> %s\n", soundex_code);
    
    soundex_encode("A", soundex_code);
    printf("   单字符 'A' -> %s\n", soundex_code);
    
    soundex_encode("B", soundex_code);
    printf("   单字符 'B' -> %s\n", soundex_code);
    
    printf("\n=== Soundex Demo 完成 ===\n");
    return 0;
}