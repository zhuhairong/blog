#include "coap_pkt.h"
#include <stdlib.h>
#include <string.h>

// 创建 CoAP 消息
coap_pkt_t* coap_pkt_create(void) {
    coap_pkt_t *pkt = calloc(1, sizeof(coap_pkt_t));
    if (pkt) {
        pkt->ver = 1;  // CoAP 版本 1
    }
    return pkt;
}

// 释放 CoAP 消息
void coap_pkt_free(coap_pkt_t *pkt) {
    if (!pkt) return;
    
    // 释放 token
    if (pkt->token) {
        free(pkt->token);
    }
    
    // 释放选项链表
    coap_option_s *opt = pkt->options;
    while (opt) {
        coap_option_s *next = opt->next;
        if (opt->value) {
            free(opt->value);
        }
        free(opt);
        opt = next;
    }
    
    // 释放 payload
    if (pkt->payload) {
        free(pkt->payload);
    }
    
    free(pkt);
}

// 编码 CoAP 消息头
static size_t coap_pkt_encode_hdr(const coap_pkt_t *pkt, uint8_t *buf) {
    buf[0] = ((pkt->ver & 0x03) << 6) | ((pkt->type & 0x03) << 4) | (pkt->tkl & 0x0F);
    buf[1] = pkt->code;
    buf[2] = (pkt->msg_id >> 8) & 0xFF;
    buf[3] = pkt->msg_id & 0xFF;
    return 4;
}

// 编码 CoAP 选项
static size_t coap_pkt_encode_options(const coap_pkt_t *pkt, uint8_t *buf) {
    size_t offset = 0;
    uint16_t prev_number = 0;
    
    coap_option_s *opt = pkt->options;
    while (opt) {
        uint16_t delta = opt->number - prev_number;
        
        // 编码 Delta
        uint8_t delta_field;
        if (delta < 13) {
            delta_field = delta;
        } else if (delta < 269) {
            delta_field = 13;
        } else {
            delta_field = 14;
        }
        
        // 编码长度
        uint8_t length_field;
        if (opt->length < 13) {
            length_field = opt->length;
        } else if (opt->length < 269) {
            length_field = 13;
        } else {
            length_field = 14;
        }
        
        buf[offset++] = (delta_field << 4) | length_field;
        
        // 扩展 Delta
        if (delta_field == 13) {
            buf[offset++] = delta - 13;
        } else if (delta_field == 14) {
            uint16_t ext_delta = delta - 269;
            buf[offset++] = (ext_delta >> 8) & 0xFF;
            buf[offset++] = ext_delta & 0xFF;
        }
        
        // 扩展长度
        if (length_field == 13) {
            buf[offset++] = opt->length - 13;
        } else if (length_field == 14) {
            uint16_t ext_length = opt->length - 269;
            buf[offset++] = (ext_length >> 8) & 0xFF;
            buf[offset++] = ext_length & 0xFF;
        }
        
        // 复制选项值
        if (opt->value && opt->length > 0) {
            memcpy(buf + offset, opt->value, opt->length);
            offset += opt->length;
        }
        
        prev_number = opt->number;
        opt = opt->next;
    }
    
    return offset;
}

// 编码 CoAP 消息
coap_error_t coap_pkt_encode(const coap_pkt_t *pkt, uint8_t *buf, size_t buf_size, size_t *written) {
    if (!pkt || !buf || !written) {
        return COAP_ERROR_INVALID_PARAM;
    }
    
    // 计算所需缓冲区大小
    size_t required = 4;  // 头部 4 字节
    required += pkt->tkl;  // Token
    
    // 选项
    coap_option_s *opt = pkt->options;
    uint16_t prev_number = 0;
    while (opt) {
        uint16_t delta = opt->number - prev_number;
        if (delta < 13) {
            required += 1;
        } else if (delta < 269) {
            required += 2;
        } else {
            required += 3;
        }
        
        if (opt->length < 13) {
            required += 1;
        } else if (opt->length < 269) {
            required += 2;
        } else {
            required += 3;
        }
        
        required += opt->length;
        prev_number = opt->number;
        opt = opt->next;
    }
    
    // Payload
    if (pkt->payload_len > 0) {
        required += 1;  // Payload marker
        required += pkt->payload_len;
    }
    
    if (buf_size < required) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    
    // 编码头部
    size_t offset = coap_pkt_encode_hdr(pkt, buf);
    
    // 编码 Token
    if (pkt->tkl > 0 && pkt->token) {
        memcpy(buf + offset, pkt->token, pkt->tkl);
        offset += pkt->tkl;
    }
    
    // 编码选项
    offset += coap_pkt_encode_options(pkt, buf + offset);
    
    // 编码 Payload
    if (pkt->payload_len > 0 && pkt->payload) {
        buf[offset++] = 0xFF;  // Payload marker
        memcpy(buf + offset, pkt->payload, pkt->payload_len);
        offset += pkt->payload_len;
    }
    
    *written = offset;
    return COAP_OK;
}

// 解码 CoAP 消息
coap_error_t coap_pkt_decode(const uint8_t *buf, size_t buf_size, coap_pkt_t *pkt) {
    if (!buf || !pkt || buf_size < 4) {
        return COAP_ERROR_INVALID_PARAM;
    }
    
    // 解码头部
    pkt->ver = (buf[0] >> 6) & 0x03;
    pkt->type = (buf[0] >> 4) & 0x03;
    pkt->tkl = buf[0] & 0x0F;
    pkt->code = buf[1];
    pkt->msg_id = ((uint16_t)buf[2] << 8) | buf[3];
    
    if (pkt->ver != 1) {
        return COAP_ERROR_INVALID_HEADER;
    }
    
    size_t offset = 4;
    
    // 解码 Token
    if (pkt->tkl > 0) {
        if (offset + pkt->tkl > buf_size) {
            return COAP_ERROR_INVALID_LENGTH;
        }
        pkt->token = malloc(pkt->tkl);
        if (!pkt->token) {
            return COAP_ERROR_MEMORY_ALLOC;
        }
        memcpy(pkt->token, buf + offset, pkt->tkl);
        offset += pkt->tkl;
    }
    
    // 解码选项
    uint16_t prev_number = 0;
    while (offset < buf_size && buf[offset] != 0xFF) {
        uint8_t byte = buf[offset++];
        uint16_t delta = (byte >> 4) & 0x0F;
        uint16_t length = byte & 0x0F;
        
        // 扩展 Delta
        if (delta == 13) {
            if (offset >= buf_size) return COAP_ERROR_INVALID_OPTION;
            delta = buf[offset++] + 13;
        } else if (delta == 14) {
            if (offset + 1 >= buf_size) return COAP_ERROR_INVALID_OPTION;
            delta = ((uint16_t)buf[offset] << 8) | buf[offset + 1];
            delta += 269;
            offset += 2;
        } else if (delta == 15) {
            return COAP_ERROR_INVALID_OPTION;
        }
        
        // 扩展长度
        if (length == 13) {
            if (offset >= buf_size) return COAP_ERROR_INVALID_OPTION;
            length = buf[offset++] + 13;
        } else if (length == 14) {
            if (offset + 1 >= buf_size) return COAP_ERROR_INVALID_OPTION;
            length = ((uint16_t)buf[offset] << 8) | buf[offset + 1];
            length += 269;
            offset += 2;
        } else if (length == 15) {
            return COAP_ERROR_INVALID_OPTION;
        }
        
        coap_option_s *opt = malloc(sizeof(coap_option_s));
        if (!opt) return COAP_ERROR_MEMORY_ALLOC;
        
        opt->number = prev_number + delta;
        opt->length = length;
        opt->next = pkt->options;
        
        if (length > 0) {
            opt->value = malloc(length);
            if (!opt->value) {
                free(opt);
                return COAP_ERROR_MEMORY_ALLOC;
            }
            if (offset + length > buf_size) {
                free(opt->value);
                free(opt);
                return COAP_ERROR_INVALID_LENGTH;
            }
            memcpy(opt->value, buf + offset, length);
            offset += length;
        } else {
            opt->value = NULL;
        }
        
        pkt->options = opt;
        prev_number = opt->number;
    }
    
    // 解码 Payload
    if (offset < buf_size && buf[offset] == 0xFF) {
        offset++;  // 跳过 payload marker
        pkt->payload_len = buf_size - offset;
        if (pkt->payload_len > 0) {
            pkt->payload = malloc(pkt->payload_len);
            if (!pkt->payload) return COAP_ERROR_MEMORY_ALLOC;
            memcpy(pkt->payload, buf + offset, pkt->payload_len);
        }
    }
    
    return COAP_OK;
}

// 添加 CoAP 选项
bool coap_pkt_add_option(coap_pkt_t *pkt, uint16_t number, const uint8_t *value, uint16_t length) {
    if (!pkt) return false;
    
    coap_option_s *opt = malloc(sizeof(coap_option_s));
    if (!opt) return false;
    
    opt->number = number;
    opt->length = length;
    opt->next = pkt->options;
    
    if (length > 0 && value) {
        opt->value = malloc(length);
        if (!opt->value) {
            free(opt);
            return false;
        }
        memcpy(opt->value, value, length);
    } else {
        opt->value = NULL;
    }
    
    pkt->options = opt;
    return true;
}

// 设置 CoAP 负载
bool coap_pkt_set_payload(coap_pkt_t *pkt, const uint8_t *payload, size_t length) {
    if (!pkt) return false;
    
    if (pkt->payload) {
        free(pkt->payload);
    }
    
    pkt->payload_len = length;
    
    if (length > 0 && payload) {
        pkt->payload = malloc(length);
        if (!pkt->payload) {
            pkt->payload_len = 0;
            return false;
        }
        memcpy(pkt->payload, payload, length);
    } else {
        pkt->payload = NULL;
    }
    
    return true;
}

// 获取 CoAP 选项
bool coap_pkt_get_option(const coap_pkt_t *pkt, uint16_t number, uint8_t index, const uint8_t **value, uint16_t *length) {
    if (!pkt || !value || !length) return false;
    
    uint8_t current_index = 0;
    coap_option_s *opt = pkt->options;
    
    while (opt) {
        if (opt->number == number) {
            if (current_index == index) {
                *value = opt->value;
                *length = opt->length;
                return true;
            }
            current_index++;
        }
        opt = opt->next;
    }
    
    return false;
}

// 获取错误信息
const char* coap_strerror(coap_error_t error) {
    switch (error) {
        case COAP_OK:
            return "Success";
        case COAP_ERROR_INVALID_HEADER:
            return "Invalid header";
        case COAP_ERROR_INVALID_OPTION:
            return "Invalid option";
        case COAP_ERROR_INVALID_LENGTH:
            return "Invalid length";
        case COAP_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        case COAP_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        default:
            return "Unknown error";
    }
}
