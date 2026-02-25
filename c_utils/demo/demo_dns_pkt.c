/**
 * DNS 报文解析演示程序
 * 
 * 功能：
 * - 构建 DNS 查询报文
 * - 解析 DNS 响应报文
 * - DNS 记录类型处理
 * - 域名解析演示
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/dns_pkt.h"

// 演示 1: 构建 DNS 查询报文
static void demo_build_query(void) {
    printf("\n=== 演示 1: 构建 DNS 查询报文 ===\n");
    
    uint8_t buf[512];
    dns_error_t error;
    
    // 构建 A 记录查询
    printf("构建 A 记录查询 (example.com):\n");
    size_t size = dns_pkt_query(buf, sizeof(buf), "example.com", 
                                 DNS_TYPE_A, DNS_CLASS_IN, 0x1234, &error);
    
    if (size > 0) {
        printf("  报文大小: %zu 字节\n", size);
        printf("  报文内容 (前 32 字节): ");
        for (size_t i = 0; i < (size < 32 ? size : 32); i++) {
            printf("%02X ", buf[i]);
        }
        printf("\n");
    } else {
        printf("  构建失败: %d\n", error);
    }
    
    // 构建 AAAA 记录查询
    printf("\n构建 AAAA 记录查询 (ipv6.google.com):\n");
    size = dns_pkt_query(buf, sizeof(buf), "ipv6.google.com", 
                        DNS_TYPE_AAAA, DNS_CLASS_IN, 0x5678, &error);
    
    if (size > 0) {
        printf("  报文大小: %zu 字节\n", size);
    }
    
    // 构建 MX 记录查询
    printf("\n构建 MX 记录查询 (gmail.com):\n");
    size = dns_pkt_query(buf, sizeof(buf), "gmail.com", 
                        DNS_TYPE_MX, DNS_CLASS_IN, 0x9ABC, &error);
    
    if (size > 0) {
        printf("  报文大小: %zu 字节\n", size);
    }
}

// 演示 2: 解析 DNS 响应报文
static void demo_parse_response(void) {
    printf("\n=== 演示 2: 解析 DNS 响应报文 ===\n");
    
    // 模拟一个 DNS 响应报文 (A 记录响应)
    uint8_t response[] = {
        0x12, 0x34, // Transaction ID
        0x81, 0x80, // Flags: Standard query response, No error
        0x00, 0x01, // Questions: 1
        0x00, 0x01, // Answer RRs: 1
        0x00, 0x00, // Authority RRs: 0
        0x00, 0x00, // Additional RRs: 0
        // Question Section
        0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
        0x03, 'c', 'o', 'm',
        0x00,       // End of name
        0x00, 0x01, // Type: A
        0x00, 0x01, // Class: IN
        // Answer Section
        0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
        0x03, 'c', 'o', 'm',
        0x00,       // End of name
        0x00, 0x01, // Type: A
        0x00, 0x01, // Class: IN
        0x00, 0x00, 0x0E, 0x10, // TTL: 3600
        0x00, 0x04, // RDLENGTH: 4
        0x5D, 0xB8, 0xD8, 0x22  // RDATA: 93.184.216.34
    };
    
    printf("模拟 DNS 响应报文解析:\n");
    printf("  报文大小: %zu 字节\n", sizeof(response));
    
    // 解析报文头部
    printf("\n  报文头部信息:\n");
    printf("    Transaction ID: 0x%04X\n", (response[0] << 8) | response[1]);
    printf("    Flags: 0x%04X (Standard response, No error)\n", 
           (response[2] << 8) | response[3]);
    printf("    Questions: %d\n", (response[4] << 8) | response[5]);
    printf("    Answers: %d\n", (response[6] << 8) | response[7]);
}

// 演示 3: DNS 记录类型
static void demo_record_types(void) {
    printf("\n=== 演示 3: DNS 记录类型 ===\n");
    
    typedef struct {
        dns_record_type_t type;
        const char *name;
        const char *desc;
    } record_type_info_t;
    
    record_type_info_t types[] = {
        {DNS_TYPE_A, "A", "IPv4 地址记录"},
        {DNS_TYPE_NS, "NS", "域名服务器记录"},
        {DNS_TYPE_CNAME, "CNAME", "别名记录"},
        {DNS_TYPE_SOA, "SOA", "授权起始记录"},
        {DNS_TYPE_PTR, "PTR", "指针记录"},
        {DNS_TYPE_MX, "MX", "邮件交换记录"},
        {DNS_TYPE_TXT, "TXT", "文本记录"},
        {DNS_TYPE_AAAA, "AAAA", "IPv6 地址记录"},
        {DNS_TYPE_SRV, "SRV", "服务定位记录"},
        {DNS_TYPE_ANY, "ANY", "所有记录类型"}
    };
    
    int num_types = sizeof(types) / sizeof(types[0]);
    
    printf("常见 DNS 记录类型:\n");
    for (int i = 0; i < num_types; i++) {
        printf("  %-5s (Type %3d) - %s\n", 
               types[i].name, types[i].type, types[i].desc);
    }
}

// 演示 4: DNS 查询流程
static void demo_query_flow(void) {
    printf("\n=== 演示 4: DNS 查询流程 ===\n");
    
    printf("DNS 查询流程:\n");
    printf("  1. 客户端构建查询报文\n");
    printf("     - 设置 Transaction ID\n");
    printf("     - 设置查询标志\n");
    printf("     - 添加问题部分 (域名 + 记录类型)\n");
    printf("\n");
    printf("  2. 发送 UDP 请求到 DNS 服务器 (端口 53)\n");
    printf("\n");
    printf("  3. 等待服务器响应\n");
    printf("\n");
    printf("  4. 解析响应报文\n");
    printf("     - 检查响应码 (RCODE)\n");
    printf("     - 提取回答记录\n");
    printf("     - 获取 IP 地址或其他记录数据\n");
    printf("\n");
    printf("  5. 缓存结果并返回给应用程序\n");
    
    printf("\nDNS 响应码 (RCODE):\n");
    printf("  0 - No Error (无错误)\n");
    printf("  1 - Format Error (格式错误)\n");
    printf("  2 - Server Failure (服务器失败)\n");
    printf("  3 - Name Error (域名不存在)\n");
    printf("  4 - Not Implemented (未实现)\n");
    printf("  5 - Refused (拒绝)\n");
}

// 演示 5: 域名编码
static void demo_name_encoding(void) {
    printf("\n=== 演示 5: 域名编码 ===\n");
    
    printf("DNS 域名编码示例 (example.com):\n");
    printf("  原始域名: example.com\n");
    printf("  编码格式: [7]example[3]com[0]\n");
    printf("  十六进制: 07 65 78 61 6D 70 6C 65 03 63 6F 6D 00\n");
    printf("\n");
    
    printf("DNS 域名编码示例 (www.example.com):\n");
    printf("  原始域名: www.example.com\n");
    printf("  编码格式: [3]www[7]example[3]com[0]\n");
    printf("  十六进制: 03 77 77 77 07 65 78 61 6D 70 6C 65 03 63 6F 6D 00\n");
    printf("\n");
    
    printf("DNS 压缩指针:\n");
    printf("  用于减少报文大小\n");
    printf("  格式: 11xxxxxx xxxxxxxx (14位偏移量)\n");
    printf("  示例: 0xC0 0x0C 表示指向报文偏移 12 的位置\n");
}

int main(void) {
    printf("========================================\n");
    printf("    DNS 报文解析演示程序\n");
    printf("========================================\n");
    
    demo_build_query();
    demo_parse_response();
    demo_record_types();
    demo_query_flow();
    demo_name_encoding();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
