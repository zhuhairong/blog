/**
 * 字节序工具演示程序
 * 
 * 功能：
 * - 字节序检测和转换
 * - 网络字节序转换
 * - 批量转换操作
 * - 内存读写操作
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../c_utils/endian_utils.h"

// 演示 1: 字节序检测
static void demo_endian_detection(void) {
    printf("\n=== 演示 1: 字节序检测 ===\n");
    
    printf("当前系统字节序:\n");
    if (endian_is_big()) {
        printf("  大端序 (Big Endian)\n");
        printf("  最高有效字节存储在最低地址\n");
    } else {
        printf("  小端序 (Little Endian)\n");
        printf("  最低有效字节存储在最低地址\n");
    }
    
    // 直观展示
    printf("\n数值 0x12345678 的内存布局:\n");
    uint32_t value = 0x12345678;
    uint8_t *bytes = (uint8_t*)&value;
    
    printf("  内存地址: ");
    for (int i = 0; i < 4; i++) {
        printf("%p ", (void*)&bytes[i]);
    }
    printf("\n");
    
    printf("  存储内容: ");
    for (int i = 0; i < 4; i++) {
        printf("0x%02X ", bytes[i]);
    }
    printf("\n");
    
    if (endian_is_big()) {
        printf("  解释: 12 56 34 78 (大端序 - 高位在前)\n");
    } else {
        printf("  解释: 78 56 34 12 (小端序 - 低位在前)\n");
    }
}

// 演示 2: 基本字节序转换
static void demo_basic_swap(void) {
    printf("\n=== 演示 2: 基本字节序转换 ===\n");
    
    // 16位转换
    printf("16位转换:\n");
    uint16_t val16 = 0x1234;
    uint16_t swapped16 = endian_swap16(val16);
    printf("  原始值: 0x%04X\n", val16);
    printf("  转换后: 0x%04X\n", swapped16);
    printf("  验证: 0x%04X -> 0x%04X\n", val16, endian_swap16(swapped16));
    
    // 32位转换
    printf("\n32位转换:\n");
    uint32_t val32 = 0x12345678;
    uint32_t swapped32 = endian_swap32(val32);
    printf("  原始值: 0x%08X\n", val32);
    printf("  转换后: 0x%08X\n", swapped32);
    printf("  验证: 0x%08X -> 0x%08X\n", val32, endian_swap32(swapped32));
    
    // 64位转换
    printf("\n64位转换:\n");
    uint64_t val64 = 0x123456789ABCDEF0ULL;
    uint64_t swapped64 = endian_swap64(val64);
    printf("  原始值: 0x%016lX\n", val64);
    printf("  转换后: 0x%016lX\n", swapped64);
    printf("  验证: 0x%016lX -> 0x%016lX\n", val64, endian_swap64(swapped64));
}

// 演示 3: 网络字节序转换
static void demo_network_byte_order(void) {
    printf("\n=== 演示 3: 网络字节序转换 ===\n");
    
    printf("网络字节序 (大端序) 转换:\n");
    printf("  网络协议使用大端序传输数据\n\n");
    
    // 主机到网络
    uint16_t host16 = 0x1234;
    uint32_t host32 = 0x12345678;
    uint64_t host64 = 0x123456789ABCDEF0ULL;
    
    printf("主机到网络:\n");
    printf("  host_to_net16(0x%04X) = 0x%04X\n", host16, host_to_net16(host16));
    printf("  host_to_net32(0x%08X) = 0x%08X\n", host32, host_to_net32(host32));
    printf("  host_to_net64(0x%016lX) = 0x%016lX\n", host64, host_to_net64(host64));
    
    // 网络到主机
    printf("\n网络到主机:\n");
    uint16_t net16 = 0x1234;
    uint32_t net32 = 0x12345678;
    uint64_t net64 = 0x123456789ABCDEF0ULL;
    
    printf("  net_to_host16(0x%04X) = 0x%04X\n", net16, net_to_host16(net16));
    printf("  net_to_host32(0x%08X) = 0x%08X\n", net32, net_to_host32(net32));
    printf("  net_to_host64(0x%016lX) = 0x%016lX\n", net64, net_to_host64(net64));
    
    // TCP/IP 端口示例
    printf("\nTCP/IP 端口示例:\n");
    uint16_t http_port = 80;
    uint16_t https_port = 443;
    printf("  HTTP 端口 %d -> 网络字节序 0x%04X\n", http_port, host_to_net16(http_port));
    printf("  HTTPS 端口 %d -> 网络字节序 0x%04X\n", https_port, host_to_net16(https_port));
}

// 演示 4: 批量转换
static void demo_batch_swap(void) {
    printf("\n=== 演示 4: 批量转换 ===\n");
    
    printf("批量字节序转换功能:\n");
    printf("  endian_swap16_batch() - 批量转换 16 位值\n");
    printf("  endian_swap32_batch() - 批量转换 32 位值\n");
    printf("  endian_swap64_batch() - 批量转换 64 位值\n");
    printf("\n");
    
    // 手动批量转换示例
    printf("手动批量转换示例:\n");
    uint16_t values16[] = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
    int n = sizeof(values16) / sizeof(values16[0]);
    
    printf("  输入: ");
    for (int i = 0; i < n; i++) {
        printf("0x%04X ", values16[i]);
    }
    printf("\n");
    
    printf("  输出: ");
    for (int i = 0; i < n; i++) {
        printf("0x%04X ", endian_swap16(values16[i]));
    }
    printf("\n");
    
    printf("\n应用场景:\n");
    printf("  - 网络数据包处理\n");
    printf("  - 二进制文件格式转换\n");
    printf("  - 批量数据处理\n");
}

// 演示 5: 内存读写
static void demo_memory_rw(void) {
    printf("\n=== 演示 5: 内存读写 ===\n");
    
    printf("内存字节序操作:\n");
    printf("  从内存读取指定字节序的值\n");
    printf("  写入指定字节序的值到内存\n");
    printf("\n");
    
    // 手动实现示例
    printf("手动实现示例:\n");
    
    // 写入大端序
    uint8_t buf[4];
    uint32_t value = 0x12345678;
    
    printf("写入大端序:\n");
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
    
    printf("  值: 0x%08X\n", value);
    printf("  内存: 0x%02X 0x%02X 0x%02X 0x%02X\n", buf[0], buf[1], buf[2], buf[3]);
    
    // 写入小端序
    printf("\n写入小端序:\n");
    buf[0] = value & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = (value >> 16) & 0xFF;
    buf[3] = (value >> 24) & 0xFF;
    
    printf("  值: 0x%08X\n", value);
    printf("  内存: 0x%02X 0x%02X 0x%02X 0x%02X\n", buf[0], buf[1], buf[2], buf[3]);
    
    // 从大端序读取
    printf("\n从大端序读取:\n");
    uint8_t big_endian[] = {0x12, 0x34, 0x56, 0x78};
    uint32_t result = ((uint32_t)big_endian[0] << 24) |
                      ((uint32_t)big_endian[1] << 16) |
                      ((uint32_t)big_endian[2] << 8) |
                      (uint32_t)big_endian[3];
    
    printf("  内存: 0x%02X 0x%02X 0x%02X 0x%02X\n", 
           big_endian[0], big_endian[1], big_endian[2], big_endian[3]);
    printf("  读取值: 0x%08X\n", result);
}

// 演示 6: 实际应用
static void demo_practical_usage(void) {
    printf("\n=== 演示 6: 实际应用场景 ===\n");
    
    printf("1. 网络协议处理\n");
    printf("   - IP 地址转换\n");
    printf("   - 端口号转换\n");
    printf("   - 协议头部字段\n");
    printf("\n");
    
    printf("2. 文件格式解析\n");
    printf("   - BMP 图像文件 (小端序)\n");
    printf("   - TIFF 图像文件 (可配置)\n");
    printf("   - WAV 音频文件 (小端序)\n");
    printf("   - MP3 文件标签\n");
    printf("\n");
    
    printf("3. 硬件通信\n");
    printf("   - 寄存器访问\n");
    printf("   - 设备驱动\n");
    printf("   - 嵌入式系统\n");
    printf("\n");
    
    printf("4. 数据序列化\n");
    printf("   - 跨平台数据交换\n");
    printf("   - 二进制文件格式\n");
    printf("   - 网络数据传输\n");
    
    printf("\n示例: 解析 BMP 文件头部\n");
    printf("  // BMP 文件使用小端序\n");
    printf("  uint16_t type = buf[0] | (buf[1] << 8);\n");
    printf("  uint32_t size = buf[2] | (buf[3] << 8) | (buf[4] << 16) | (buf[5] << 24);\n");
    printf("  // 'BM' = 0x4D42 (小端序)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    字节序工具演示程序\n");
    printf("========================================\n");
    
    demo_endian_detection();
    demo_basic_swap();
    demo_network_byte_order();
    demo_batch_swap();
    demo_memory_rw();
    demo_practical_usage();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
