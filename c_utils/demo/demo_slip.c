#include <stdio.h>
#include "../c_utils/slip.h"

void print_hex(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

int main() {
    printf("=== SLIP Demo ===\n\n");
    
    // 测试数据
    unsigned char test_data[] = {0x01, 0x02, 0xC0, 0x03, 0xDB, 0x04, 0x05};
    size_t data_len = sizeof(test_data);
    
    printf("原始数据: ");
    print_hex(test_data, data_len);
    
    // 测试基本SLIP编码
    printf("\n1. 基本SLIP编码:\n");
    unsigned char encoded[64];
    size_t encoded_len = slip_encode(test_data, data_len, encoded);
    printf("   编码后: ");
    print_hex(encoded, encoded_len);
    
    // 测试基本SLIP解码
    printf("\n2. 基本SLIP解码:\n");
    unsigned char decoded[64];
    slip_state_t state;
    slip_state_init(&state);
    size_t decoded_len = slip_decode(encoded, encoded_len, decoded, sizeof(decoded), &state);
    printf("   解码后: ");
    print_hex(decoded, decoded_len);
    
    // 测试增强版SLIP编码
    printf("\n3. 增强版SLIP编码:\n");
    slip_config_t config;
    slip_config_init(&config);
    config.add_start_delimiter = true;
    config.add_end_delimiter = true;
    
    slip_state_t state_ex;
    slip_state_init(&state_ex);
    
    unsigned char encoded_ex[64];
    size_t encoded_ex_len = slip_encode_ex(test_data, data_len, encoded_ex, sizeof(encoded_ex), &config, &state_ex);
    printf("   编码后: ");
    print_hex(encoded_ex, encoded_ex_len);
    
    // 测试增强版SLIP解码
    printf("\n4. 增强版SLIP解码:\n");
    unsigned char decoded_ex[64];
    size_t decoded_ex_len = slip_decode_ex(encoded_ex, encoded_ex_len, decoded_ex, sizeof(decoded_ex), &config, &state_ex);
    printf("   解码后: ");
    print_hex(decoded_ex, decoded_ex_len);
    
    // 测试计算最大编码大小
    printf("\n5. 计算最大编码大小:\n");
    size_t max_size = slip_calculate_max_encoded_size(data_len, &config);
    printf("   输入长度: %zu 字节\n", data_len);
    printf("   最大编码大小: %zu 字节\n", max_size);
    
    // 测试统计信息
    printf("\n6. 统计信息:\n");
    printf("   总编码字节数: %zu\n", state_ex.total_encoded);
    printf("   总解码字节数: %zu\n", state_ex.total_decoded);
    printf("   编码包数: %zu\n", state_ex.packets_encoded);
    printf("   解码包数: %zu\n", state_ex.packets_decoded);
    
    // 测试边界情况
    printf("\n7. 边界情况测试:\n");
    
    // 空数据
    unsigned char empty_data[] = {};
    size_t empty_len = slip_encode(empty_data, 0, encoded);
    printf("   空数据编码长度: %zu\n", empty_len);
    
    // 只包含特殊字符的数据
    unsigned char special_data[] = {SLIP_END, SLIP_ESC};
    size_t special_len = sizeof(special_data);
    size_t special_encoded_len = slip_encode(special_data, special_len, encoded);
    printf("   特殊字符编码前: ");
    print_hex(special_data, special_len);
    printf("   特殊字符编码后: ");
    print_hex(encoded, special_encoded_len);
    
    printf("\n=== SLIP Demo 完成 ===\n");
    return 0;
}