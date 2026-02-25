/**
 * 字节序工具演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/endian_utils.h"

static void print_hex16(const char *label, uint16_t v) {
    printf("%s: 0x%04X\n", label, v);
}

static void print_hex32(const char *label, uint32_t v) {
    printf("%s: 0x%08X\n", label, v);
}

static void print_hex64(const char *label, uint64_t v) {
    printf("%s: 0x%016lX\n", label, v);
}

static void demo_basic_swap(void) {
    printf("\n=== 演示 1: 基本字节序转换 ===\n");

    printf("当前系统字节序: %s\n\n", endian_is_big() ? "大端 (Big Endian)" : "小端 (Little Endian)");

    printf("16位转换:\n");
    uint16_t v16 = 0x1234;
    print_hex16("  原始值", v16);
    print_hex16("  交换后", endian_swap16(v16));
    print_hex16("  再交换", endian_swap16(endian_swap16(v16)));
    printf("\n");

    printf("32位转换:\n");
    uint32_t v32 = 0x12345678;
    print_hex32("  原始值", v32);
    print_hex32("  交换后", endian_swap32(v32));
    print_hex32("  再交换", endian_swap32(endian_swap32(v32)));
    printf("\n");

    printf("64位转换:\n");
    uint64_t v64 = 0x123456789ABCDEF0ULL;
    print_hex64("  原始值", v64);
    print_hex64("  交换后", endian_swap64(v64));
    print_hex64("  再交换", endian_swap64(endian_swap64(v64)));
}

static void demo_network_byte_order(void) {
    printf("\n=== 演示 2: 网络字节序转换 ===\n");

    printf("主机序与网络序转换:\n\n");

    uint16_t host16 = 0x1234;
    uint16_t net16 = host_to_net16(host16);
    printf("16位:\n");
    print_hex16("  主机序", host16);
    print_hex16("  网络序", net16);
    print_hex16("  转回来", net_to_host16(net16));
    printf("\n");

    uint32_t host32 = 0x12345678;
    uint32_t net32 = host_to_net32(host32);
    printf("32位:\n");
    print_hex32("  主机序", host32);
    print_hex32("  网络序", net32);
    print_hex32("  转回来", net_to_host32(net32));
    printf("\n");

    uint64_t host64 = 0x123456789ABCDEF0ULL;
    uint64_t net64 = host_to_net64(host64);
    printf("64位:\n");
    print_hex64("  主机序", host64);
    print_hex64("  网络序", net64);
    print_hex64("  转回来", net_to_host64(net64));
}

static void demo_practical_usage(void) {
    printf("\n=== 演示 3: 实际应用场景 ===\n");

    printf("1. 网络协议数据转换:\n");
    uint16_t port = 8080;
    uint32_t ip = 0xC0A80001; // 192.168.0.1
    printf("   端口号 %d (0x%04X) -> 网络序 0x%04X\n", port, port, host_to_net16(port));
    printf("   IP 0x%08X -> 网络序 0x%08X\n", ip, host_to_net32(ip));

    printf("\n2. 文件格式数据:\n");
    uint32_t magic = 0x89504E47; // PNG 文件魔数
    printf("   PNG 魔数 0x%08X\n", magic);
    printf("   交换后 0x%08X\n", endian_swap32(magic));

    printf("\n3. 跨平台数据交换:\n");
    uint16_t data[] = {0x0102, 0x0304, 0x0506};
    printf("   原始数据: ");
    for (int i = 0; i < 3; i++) printf("0x%04X ", data[i]);
    printf("\n   交换后:   ");
    for (int i = 0; i < 3; i++) printf("0x%04X ", endian_swap16(data[i]));
    printf("\n");
}

int main(void) {
    printf("========================================\n");
    printf("    字节序工具演示\n");
    printf("========================================\n");

    demo_basic_swap();
    demo_network_byte_order();
    demo_practical_usage();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
