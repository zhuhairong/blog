#ifndef C_UTILS_DNS_PKT_H
#define C_UTILS_DNS_PKT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// DNS 错误码
typedef enum {
    DNS_OK = 0,
    DNS_ERROR_INVALID_PARAM,
    DNS_ERROR_INVALID_PACKET,
    DNS_ERROR_MEMORY_ALLOC,
    DNS_ERROR_BUFFER_TOO_SMALL,
    DNS_ERROR_UNSUPPORTED_RECORD,
    DNS_ERROR_NAME_TOO_LONG,
    DNS_ERROR_QUERY_FAILED
} dns_error_t;

// DNS 记录类型
typedef enum {
    DNS_TYPE_A = 1,
    DNS_TYPE_NS = 2,
    DNS_TYPE_CNAME = 5,
    DNS_TYPE_SOA = 6,
    DNS_TYPE_PTR = 12,
    DNS_TYPE_MX = 15,
    DNS_TYPE_TXT = 16,
    DNS_TYPE_AAAA = 28,
    DNS_TYPE_SRV = 33,
    DNS_TYPE_ANY = 255
} dns_record_type_t;

// DNS 类
typedef enum {
    DNS_CLASS_IN = 1,
    DNS_CLASS_CS = 2,
    DNS_CLASS_CH = 3,
    DNS_CLASS_HS = 4,
    DNS_CLASS_ANY = 255
} dns_class_t;

// DNS 头
typedef struct {
    uint16_t id;
    uint8_t qr : 1;
    uint8_t opcode : 4;
    uint8_t aa : 1;
    uint8_t tc : 1;
    uint8_t rd : 1;
    uint8_t ra : 1;
    uint8_t z : 3;
    uint8_t rcode : 4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} dns_header_t;

// DNS 问题
typedef struct {
    char *name;
    uint16_t type;
    uint16_t class;
} dns_question_t;

// DNS 资源记录
typedef struct {
    char *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    void *rdata;
} dns_resource_record_t;

// DNS 报文
typedef struct {
    dns_header_t header;
    dns_question_t *questions;
    dns_resource_record_t *answers;
    dns_resource_record_t *authority;
    dns_resource_record_t *additional;
    bool has_error;
    dns_error_t error;
    char error_msg[256];
} dns_pkt_t;

// 构建 DNS 查询报文
// buf: 输出缓冲区
// buf_size: 缓冲区大小
// hostname: 主机名
// type: 记录类型
// class: 类
// id: 查询 ID（0 表示自动生成）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回报文大小，失败返回 0
size_t dns_pkt_query(uint8_t *buf, size_t buf_size, const char *hostname, dns_record_type_t type, dns_class_t class, uint16_t id, dns_error_t *error);

// 解析 DNS 报文
// buf: 输入缓冲区
// buf_size: 缓冲区大小
// pkt: 解析结果（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dns_pkt_parse(const uint8_t *buf, size_t buf_size, dns_pkt_t *pkt, dns_error_t *error);

// 释放 DNS 报文
// pkt: DNS 报文
void dns_pkt_free(dns_pkt_t *pkt);

// 获取 A 记录 IP 地址
// pkt: DNS 报文
// index: 记录索引
// ip: IP 地址缓冲区（至少 16 字节）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dns_pkt_get_a_record(const dns_pkt_t *pkt, size_t index, char *ip, dns_error_t *error);

// 获取 AAAA 记录 IP 地址
// pkt: DNS 报文
// index: 记录索引
// ip: IP 地址缓冲区（至少 40 字节）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dns_pkt_get_aaaa_record(const dns_pkt_t *pkt, size_t index, char *ip, dns_error_t *error);

// 获取 CNAME 记录
// pkt: DNS 报文
// index: 记录索引
// cname: CNAME 缓冲区
// cname_size: CNAME 缓冲区大小
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dns_pkt_get_cname_record(const dns_pkt_t *pkt, size_t index, char *cname, size_t cname_size, dns_error_t *error);

// 检查 DNS 报文是否有错误
// pkt: DNS 报文
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool dns_pkt_has_error(const dns_pkt_t *pkt, dns_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* dns_strerror(dns_error_t error);

#endif // C_UTILS_DNS_PKT_H
