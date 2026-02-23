#ifndef C_UTILS_COAP_PKT_H
#define C_UTILS_COAP_PKT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// CoAP 错误码
typedef enum {
    COAP_OK = 0,
    COAP_ERROR_INVALID_HEADER,
    COAP_ERROR_INVALID_OPTION,
    COAP_ERROR_INVALID_LENGTH,
    COAP_ERROR_MEMORY_ALLOC,
    COAP_ERROR_INVALID_PARAM
} coap_error_t;

// CoAP 消息类型
typedef enum {
    COAP_TYPE_CON = 0,  // 确认消息
    COAP_TYPE_NON = 1,  // 非确认消息
    COAP_TYPE_ACK = 2,  // 确认消息
    COAP_TYPE_RST = 3   // 复位消息
} coap_type_t;

// CoAP 方法码
typedef enum {
    COAP_METHOD_GET     = 1,
    COAP_METHOD_POST    = 2,
    COAP_METHOD_PUT     = 3,
    COAP_METHOD_DELETE  = 4
} coap_method_t;

// CoAP 响应码
typedef enum {
    COAP_CODE_200_OK            = 65,   // 2.00 OK
    COAP_CODE_201_CREATED       = 66,   // 2.01 Created
    COAP_CODE_202_DELETED       = 67,   // 2.02 Deleted
    COAP_CODE_203_VALID         = 68,   // 2.03 Valid
    COAP_CODE_204_CHANGED       = 69,   // 2.04 Changed
    COAP_CODE_205_CONTENT       = 70,   // 2.05 Content
    COAP_CODE_400_BAD_REQUEST   = 128,  // 4.00 Bad Request
    COAP_CODE_401_UNAUTHORIZED  = 129,  // 4.01 Unauthorized
    COAP_CODE_402_BAD_OPTION    = 130,  // 4.02 Bad Option
    COAP_CODE_403_FORBIDDEN     = 131,  // 4.03 Forbidden
    COAP_CODE_404_NOT_FOUND     = 132,  // 4.04 Not Found
    COAP_CODE_405_METHOD_NOT_ALLOWED = 133,  // 4.05 Method Not Allowed
    COAP_CODE_406_NOT_ACCEPTABLE = 134,  // 4.06 Not Acceptable
    COAP_CODE_500_INTERNAL_SERVER_ERROR = 160,  // 5.00 Internal Server Error
    COAP_CODE_501_NOT_IMPLEMENTED = 161,  // 5.01 Not Implemented
    COAP_CODE_502_BAD_GATEWAY   = 162,  // 5.02 Bad Gateway
    COAP_CODE_503_SERVICE_UNAVAILABLE = 163,  // 5.03 Service Unavailable
    COAP_CODE_504_GATEWAY_TIMEOUT = 164,  // 5.04 Gateway Timeout
    COAP_CODE_505_PROXYING_NOT_SUPPORTED = 165  // 5.05 Proxying Not Supported
} coap_code_t;

// CoAP 选项编号
typedef enum {
    COAP_OPTION_IF_MATCH      = 1,
    COAP_OPTION_URI_HOST      = 3,
    COAP_OPTION_ETAG          = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_OBSERVE       = 6,
    COAP_OPTION_URI_PORT      = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH      = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE       = 14,
    COAP_OPTION_URI_QUERY     = 15,
    COAP_OPTION_ACCEPT        = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_BLOCK2        = 23,
    COAP_OPTION_BLOCK1        = 27,
    COAP_OPTION_PROXY_URI     = 35,
    COAP_OPTION_PROXY_SCHEME  = 39,
    COAP_OPTION_SIZE1         = 60
} coap_option_t;

// CoAP 选项
typedef struct {
    uint16_t number;
    uint16_t length;
    uint8_t *value;
    struct coap_option_s *next;
} coap_option_s;

// CoAP 消息
typedef struct {
    uint8_t ver;
    coap_type_t type;
    uint8_t tkl;
    uint8_t code;
    uint16_t msg_id;
    uint8_t *token;
    coap_option_s *options;
    size_t payload_len;
    uint8_t *payload;
} coap_pkt_t;

// 创建 CoAP 消息
// 返回: 成功返回消息指针，失败返回 NULL
coap_pkt_t* coap_pkt_create(void);

// 释放 CoAP 消息
// pkt: CoAP 消息
void coap_pkt_free(coap_pkt_t *pkt);

// 编码 CoAP 消息
// pkt: CoAP 消息
// buf: 输出缓冲区
// buf_size: 缓冲区大小
// written: 实际写入字节数（输出参数）
// 返回: 成功返回 COAP_OK，失败返回错误码
coap_error_t coap_pkt_encode(const coap_pkt_t *pkt, uint8_t *buf, size_t buf_size, size_t *written);

// 解码 CoAP 消息
// buf: 输入缓冲区
// buf_size: 缓冲区大小
// pkt: CoAP 消息（输出参数）
// 返回: 成功返回 COAP_OK，失败返回错误码
coap_error_t coap_pkt_decode(const uint8_t *buf, size_t buf_size, coap_pkt_t *pkt);

// 添加 CoAP 选项
// pkt: CoAP 消息
// number: 选项编号
// value: 选项值
// length: 选项长度
// 返回: 成功返回 true，失败返回 false
bool coap_pkt_add_option(coap_pkt_t *pkt, uint16_t number, const uint8_t *value, uint16_t length);

// 设置 CoAP 负载
// pkt: CoAP 消息
// payload: 负载数据
// length: 负载长度
// 返回: 成功返回 true，失败返回 false
bool coap_pkt_set_payload(coap_pkt_t *pkt, const uint8_t *payload, size_t length);

// 获取 CoAP 选项
// pkt: CoAP 消息
// number: 选项编号
// index: 选项索引（相同编号的多个选项）
// value: 选项值（输出参数）
// length: 选项长度（输出参数）
// 返回: 成功返回 true，失败返回 false
bool coap_pkt_get_option(const coap_pkt_t *pkt, uint16_t number, uint8_t index, const uint8_t **value, uint16_t *length);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* coap_strerror(coap_error_t error);

#endif // C_UTILS_COAP_PKT_H
