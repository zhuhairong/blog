#include <stdio.h>
#include "../c_utils/url_codec.h"

int main() {
    printf("=== URL Demo ===\n\n");
    
    // 测试URL编码
    printf("1. URL编码:\n");
    const char* raw_str = "Hello World! 你好，世界！";
    char encoded[256];
    
    url_encode(raw_str, encoded);
    printf("   原字符串: %s\n", raw_str);
    printf("   编码后: %s\n", encoded);
    
    // 测试URL解码
    printf("\n2. URL解码:\n");
    const char* encoded_str = "Hello%20World%21%20%E4%BD%A0%E5%A5%BD%EF%BC%8C%E4%B8%96%E7%95%8C%EF%BC%81";
    char decoded[256];
    
    url_decode(encoded_str, decoded);
    printf("   编码字符串: %s\n", encoded_str);
    printf("   解码后: %s\n", decoded);
    
    printf("\n=== URL Demo 完成 ===\n");
    return 0;
}