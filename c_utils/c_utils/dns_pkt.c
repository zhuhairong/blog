#include "dns_pkt.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

// 构建 DNS 查询报文
size_t dns_pkt_query(uint8_t *buf, size_t buf_size, const char *hostname, 
                     dns_record_type_t type, dns_class_t class, uint16_t id, 
                     dns_error_t *error) {
    if (!buf || !hostname || buf_size < 512) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return 0;
    }
    
    if (strlen(hostname) > 253) {
        if (error) *error = DNS_ERROR_NAME_TOO_LONG;
        return 0;
    }
    
    uint8_t *p = buf;
    
    // DNS 头部
    uint16_t pkt_id = (id == 0) ? (uint16_t)(rand() & 0xFFFF) : id;
    *p++ = (uint8_t)(pkt_id >> 8);
    *p++ = (uint8_t)(pkt_id & 0xFF);
    
    *p++ = 0x01; // RD = 1 (递归查询)
    *p++ = 0x00;
    
    *p++ = 0x00; // Questions = 1
    *p++ = 0x01;
    
    *p++ = 0x00; // Answer RRs = 0
    *p++ = 0x00;
    
    *p++ = 0x00; // Authority RRs = 0
    *p++ = 0x00;
    
    *p++ = 0x00; // Additional RRs = 0
    *p++ = 0x00;
    
    // 查询名称
    const char *start = hostname;
    const char *end;
    while ((end = strchr(start, '.')) != NULL) {
        size_t len = end - start;
        if (len > 63) {
            if (error) *error = DNS_ERROR_NAME_TOO_LONG;
            return 0;
        }
        *p++ = (uint8_t)len;
        memcpy(p, start, len);
        p += len;
        start = end + 1;
    }
    
    size_t final_len = strlen(start);
    if (final_len > 0) {
        *p++ = (uint8_t)final_len;
        memcpy(p, start, final_len);
        p += final_len;
    }
    *p++ = 0; // 名称结束
    
    // 查询类型
    *p++ = 0;
    *p++ = (uint8_t)type;
    
    // 查询类
    *p++ = 0;
    *p++ = (uint8_t)class;
    
    if (error) *error = DNS_OK;
    return p - buf;
}

// 解析 DNS 名称
static int dns_parse_name(const uint8_t *buf, size_t buf_size, size_t offset, 
                          char *name, size_t name_size, size_t *next_offset) {
    if (offset >= buf_size) return -1;
    
    size_t name_pos = 0;
    size_t pos = offset;
    bool jumped = false;
    size_t jump_pos = 0;
    
    while (pos < buf_size) {
        uint8_t len = buf[pos];
        
        if (len == 0) {
            pos++;
            break;
        }
        
        // 检查是否是压缩指针
        if ((len & 0xC0) == 0xC0) {
            if (pos + 1 >= buf_size) return -1;
            if (!jumped) {
                jump_pos = pos + 2;
                jumped = true;
            }
            pos = ((len & 0x3F) << 8) | buf[pos + 1];
            continue;
        }
        
        if (len > 63) return -1;
        
        pos++;
        
        if (name_pos > 0 && name_pos < name_size - 1) {
            name[name_pos++] = '.';
        }
        
        for (size_t i = 0; i < len && pos < buf_size && name_pos < name_size - 1; i++) {
            name[name_pos++] = (char)buf[pos++];
        }
    }
    
    if (name_pos < name_size) {
        name[name_pos] = '\0';
    }
    
    *next_offset = jumped ? jump_pos : pos;
    return 0;
}

// 解析 DNS 报文
bool dns_pkt_parse(const uint8_t *buf, size_t buf_size, dns_pkt_t *pkt, dns_error_t *error) {
    if (!buf || !pkt || buf_size < 12) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    memset(pkt, 0, sizeof(dns_pkt_t));
    
    const uint8_t *p = buf;
    
    // 解析头部
    pkt->header.id = (uint16_t)((p[0] << 8) | p[1]);
    pkt->header.qr = (p[2] >> 7) & 0x01;
    pkt->header.opcode = (p[2] >> 3) & 0x0F;
    pkt->header.aa = (p[2] >> 2) & 0x01;
    pkt->header.tc = (p[2] >> 1) & 0x01;
    pkt->header.rd = p[2] & 0x01;
    pkt->header.ra = (p[3] >> 7) & 0x01;
    pkt->header.z = (p[3] >> 4) & 0x07;
    pkt->header.rcode = p[3] & 0x0F;
    pkt->header.qdcount = (uint16_t)((p[4] << 8) | p[5]);
    pkt->header.ancount = (uint16_t)((p[6] << 8) | p[7]);
    pkt->header.nscount = (uint16_t)((p[8] << 8) | p[9]);
    pkt->header.arcount = (uint16_t)((p[10] << 8) | p[11]);
    
    p += 12;
    
    // 检查响应码
    if (pkt->header.rcode != 0) {
        pkt->has_error = true;
        pkt->error = DNS_ERROR_QUERY_FAILED;
        snprintf(pkt->error_msg, sizeof(pkt->error_msg), "DNS response code: %d", pkt->header.rcode);
    }
    
    size_t offset = 12;
    
    // 解析问题部分
    if (pkt->header.qdcount > 0) {
        pkt->questions = calloc(pkt->header.qdcount, sizeof(dns_question_t));
        if (!pkt->questions) {
            if (error) *error = DNS_ERROR_MEMORY_ALLOC;
            return false;
        }
        
        for (uint16_t i = 0; i < pkt->header.qdcount; i++) {
            char name[256];
            size_t next_offset;
            if (dns_parse_name(buf, buf_size, offset, name, sizeof(name), &next_offset) < 0) {
                if (error) *error = DNS_ERROR_INVALID_PACKET;
                return false;
            }
            
            pkt->questions[i].name = strdup(name);
            
            if (next_offset + 4 > buf_size) {
                if (error) *error = DNS_ERROR_INVALID_PACKET;
                return false;
            }
            
            pkt->questions[i].type = (uint16_t)((buf[next_offset] << 8) | buf[next_offset + 1]);
            pkt->questions[i].class = (uint16_t)((buf[next_offset + 2] << 8) | buf[next_offset + 3]);
            offset = next_offset + 4;
        }
    }
    
    // 解析回答部分
    if (pkt->header.ancount > 0) {
        pkt->answers = calloc(pkt->header.ancount, sizeof(dns_resource_record_t));
        if (!pkt->answers) {
            if (error) *error = DNS_ERROR_MEMORY_ALLOC;
            return false;
        }
        
        for (uint16_t i = 0; i < pkt->header.ancount; i++) {
            char name[256];
            size_t next_offset;
            if (dns_parse_name(buf, buf_size, offset, name, sizeof(name), &next_offset) < 0) {
                if (error) *error = DNS_ERROR_INVALID_PACKET;
                return false;
            }
            
            pkt->answers[i].name = strdup(name);
            
            if (next_offset + 10 > buf_size) {
                if (error) *error = DNS_ERROR_INVALID_PACKET;
                return false;
            }
            
            pkt->answers[i].type = (uint16_t)((buf[next_offset] << 8) | buf[next_offset + 1]);
            pkt->answers[i].class = (uint16_t)((buf[next_offset + 2] << 8) | buf[next_offset + 3]);
            pkt->answers[i].ttl = ((uint32_t)buf[next_offset + 4] << 24) |
                                  ((uint32_t)buf[next_offset + 5] << 16) |
                                  ((uint32_t)buf[next_offset + 6] << 8) |
                                  (uint32_t)buf[next_offset + 7];
            pkt->answers[i].rdlength = (uint16_t)((buf[next_offset + 8] << 8) | buf[next_offset + 9]);
            
            offset = next_offset + 10;
            
            if (offset + pkt->answers[i].rdlength > buf_size) {
                if (error) *error = DNS_ERROR_INVALID_PACKET;
                return false;
            }
            
            pkt->answers[i].rdata = malloc(pkt->answers[i].rdlength);
            if (pkt->answers[i].rdata) {
                memcpy(pkt->answers[i].rdata, buf + offset, pkt->answers[i].rdlength);
            }
            offset += pkt->answers[i].rdlength;
        }
    }
    
    if (error) *error = DNS_OK;
    return true;
}

// 释放 DNS 报文
void dns_pkt_free(dns_pkt_t *pkt) {
    if (!pkt) return;
    
    // 释放问题部分
    if (pkt->questions) {
        for (uint16_t i = 0; i < pkt->header.qdcount; i++) {
            free(pkt->questions[i].name);
        }
        free(pkt->questions);
    }
    
    // 释放回答部分
    if (pkt->answers) {
        for (uint16_t i = 0; i < pkt->header.ancount; i++) {
            free(pkt->answers[i].name);
            free(pkt->answers[i].rdata);
        }
        free(pkt->answers);
    }
    
    // 释放权威部分
    if (pkt->authority) {
        for (uint16_t i = 0; i < pkt->header.nscount; i++) {
            free(pkt->authority[i].name);
            free(pkt->authority[i].rdata);
        }
        free(pkt->authority);
    }
    
    // 释放附加部分
    if (pkt->additional) {
        for (uint16_t i = 0; i < pkt->header.arcount; i++) {
            free(pkt->additional[i].name);
            free(pkt->additional[i].rdata);
        }
        free(pkt->additional);
    }
    
    memset(pkt, 0, sizeof(dns_pkt_t));
}

// 获取 A 记录 IP 地址
bool dns_pkt_get_a_record(const dns_pkt_t *pkt, size_t index, char *ip, dns_error_t *error) {
    if (!pkt || !ip) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (index >= pkt->header.ancount) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    dns_resource_record_t *rr = &pkt->answers[index];
    if (rr->type != DNS_TYPE_A || rr->rdlength != 4) {
        if (error) *error = DNS_ERROR_UNSUPPORTED_RECORD;
        return false;
    }
    
    uint8_t *addr = (uint8_t *)rr->rdata;
    snprintf(ip, 16, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
    
    if (error) *error = DNS_OK;
    return true;
}

// 获取 AAAA 记录 IP 地址
bool dns_pkt_get_aaaa_record(const dns_pkt_t *pkt, size_t index, char *ip, dns_error_t *error) {
    if (!pkt || !ip) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (index >= pkt->header.ancount) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    dns_resource_record_t *rr = &pkt->answers[index];
    if (rr->type != DNS_TYPE_AAAA || rr->rdlength != 16) {
        if (error) *error = DNS_ERROR_UNSUPPORTED_RECORD;
        return false;
    }
    
    uint16_t *addr = (uint16_t *)rr->rdata;
    snprintf(ip, 40, "%x:%x:%x:%x:%x:%x:%x:%x",
             ntohs(addr[0]), ntohs(addr[1]), ntohs(addr[2]), ntohs(addr[3]),
             ntohs(addr[4]), ntohs(addr[5]), ntohs(addr[6]), ntohs(addr[7]));
    
    if (error) *error = DNS_OK;
    return true;
}

// 获取 CNAME 记录
bool dns_pkt_get_cname_record(const dns_pkt_t *pkt, size_t index, char *cname, 
                              size_t cname_size, dns_error_t *error) {
    if (!pkt || !cname || cname_size == 0) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (index >= pkt->header.ancount) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return false;
    }
    
    dns_resource_record_t *rr = &pkt->answers[index];
    if (rr->type != DNS_TYPE_CNAME) {
        if (error) *error = DNS_ERROR_UNSUPPORTED_RECORD;
        return false;
    }
    
    // 解析 CNAME 数据（压缩名称）
    // 这里简化处理，实际需要解析压缩格式
    strncpy(cname, rr->name, cname_size - 1);
    cname[cname_size - 1] = '\0';
    
    if (error) *error = DNS_OK;
    return true;
}

// 检查 DNS 报文是否有错误
bool dns_pkt_has_error(const dns_pkt_t *pkt, dns_error_t *error, const char **error_msg) {
    if (!pkt) {
        if (error) *error = DNS_ERROR_INVALID_PARAM;
        return true;
    }
    
    if (pkt->has_error) {
        if (error) *error = pkt->error;
        if (error_msg) *error_msg = pkt->error_msg;
        return true;
    }
    
    if (error) *error = DNS_OK;
    if (error_msg) *error_msg = NULL;
    return false;
}

// 获取错误信息
const char* dns_strerror(dns_error_t error) {
    switch (error) {
        case DNS_OK: return "Success";
        case DNS_ERROR_INVALID_PARAM: return "Invalid parameter";
        case DNS_ERROR_INVALID_PACKET: return "Invalid packet";
        case DNS_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case DNS_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case DNS_ERROR_UNSUPPORTED_RECORD: return "Unsupported record type";
        case DNS_ERROR_NAME_TOO_LONG: return "Name too long";
        case DNS_ERROR_QUERY_FAILED: return "Query failed";
        default: return "Unknown error";
    }
}
