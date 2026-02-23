#ifndef C_UTILS_MQTT_PKT_H
#define C_UTILS_MQTT_PKT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief MQTT 报文类型
 */
typedef enum {
    MQTT_PKT_TYPE_CONNECT = 1,       /**< 连接请求 */
    MQTT_PKT_TYPE_CONNACK,           /**< 连接确认 */
    MQTT_PKT_TYPE_PUBLISH,           /**< 发布消息 */
    MQTT_PKT_TYPE_PUBACK,            /**< 发布确认 */
    MQTT_PKT_TYPE_PUBREC,            /**< 发布接收 */
    MQTT_PKT_TYPE_PUBREL,            /**< 发布释放 */
    MQTT_PKT_TYPE_PUBCOMP,           /**< 发布完成 */
    MQTT_PKT_TYPE_SUBSCRIBE,         /**< 订阅请求 */
    MQTT_PKT_TYPE_SUBACK,            /**< 订阅确认 */
    MQTT_PKT_TYPE_UNSUBSCRIBE,       /**< 取消订阅 */
    MQTT_PKT_TYPE_UNSUBACK,          /**< 取消订阅确认 */
    MQTT_PKT_TYPE_PINGREQ,           /**<  ping 请求 */
    MQTT_PKT_TYPE_PINGRESP,          /**<  ping 响应 */
    MQTT_PKT_TYPE_DISCONNECT         /**< 断开连接 */
} mqtt_pkt_type_t;

/**
 * @brief MQTT 错误码
 */
typedef enum {
    MQTT_PKT_OK = 0,                 /**< 成功 */
    MQTT_PKT_ERROR_NULL_PTR,         /**< 空指针错误 */
    MQTT_PKT_ERROR_INVALID_ARGS,     /**< 无效参数 */
    MQTT_PKT_ERROR_BUFFER_TOO_SMALL, /**< 缓冲区太小 */
    MQTT_PKT_ERROR_INVALID_PACKET,   /**< 无效报文 */
    MQTT_PKT_ERROR_UNSUPPORTED,      /**< 不支持的功能 */
    MQTT_PKT_ERROR_MAX               /**< 最大错误码 */
} mqtt_pkt_error_t;

/**
 * @brief MQTT Connect 配置
 */
typedef struct {
    const char *client_id;           /**< 客户端 ID */
    const char *username;            /**< 用户名 (可选) */
    const char *password;            /**< 密码 (可选) */
    const char *will_topic;          /**< 遗嘱主题 (可选) */
    const char *will_message;        /**< 遗嘱消息 (可选) */
    uint8_t will_qos;                /**< 遗嘱 QoS (0-2) */
    bool will_retain;                /**< 遗嘱保留标志 */
    bool clean_session;              /**< 清理会话标志 */
    uint16_t keep_alive;             /**< 保活时间 (秒) */
    uint8_t protocol_level;          /**< 协议级别 (4=MQTT 3.1.1, 5=MQTT 5.0) */
} mqtt_pkt_connect_config_t;

/**
 * @brief MQTT Publish 配置
 */
typedef struct {
    const char *topic;               /**< 主题 */
    const void *payload;             /**< 载荷数据 */
    size_t payload_len;              /**< 载荷长度 */
    uint8_t qos;                     /**< QoS (0-2) */
    bool retain;                     /**< 保留标志 */
    uint16_t packet_id;              /**< 报文 ID (QoS > 0 时需要) */
} mqtt_pkt_publish_config_t;

/**
 * @brief MQTT 订阅主题
 */
typedef struct {
    const char *topic_filter;        /**< 主题过滤器 */
    uint8_t qos;                     /**< 请求的 QoS */
} mqtt_pkt_topic_t;

/**
 * @brief MQTT 报文解析结果
 */
typedef struct {
    mqtt_pkt_type_t type;            /**< 报文类型 */
    uint8_t flags;                   /**< 标志位 */
    size_t remaining_length;         /**< 剩余长度 */
    const uint8_t *payload;          /**< 载荷指针 */
    size_t payload_len;              /**< 载荷长度 */
    // 特定报文类型的字段
    union {
        struct {
            uint8_t connack_flags;   /**< CONNACK 标志 */
            uint8_t return_code;     /**< 返回码 */
        } connack;
        struct {
            char topic[128];         /**< PUBLISH 主题 */
            uint16_t packet_id;      /**< 报文 ID */
            const uint8_t *payload;  /**< 载荷 */
            size_t payload_len;      /**< 载荷长度 */
        } publish;
        struct {
            uint16_t packet_id;      /**< 报文 ID */
            uint8_t *return_codes;   /**< 返回码数组 */
            size_t count;            /**< 返回码数量 */
        } suback;
    } data;
} mqtt_pkt_parse_result_t;

/**
 * @brief 获取默认 Connect 配置
 * @return 默认配置
 */
mqtt_pkt_connect_config_t mqtt_pkt_default_connect_config(void);

/**
 * @brief 获取默认 Publish 配置
 * @return 默认配置
 */
mqtt_pkt_publish_config_t mqtt_pkt_default_publish_config(void);

/**
 * @brief 构建 MQTT Connect 报文
 * @param buf 缓冲区
 * @param client_id 客户端 ID
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_connect(uint8_t *buf, const char *client_id);

/**
 * @brief 使用自定义配置构建 MQTT Connect 报文
 * @param buf 缓冲区
 * @param buf_size 缓冲区大小
 * @param config 配置选项
 * @param error 错误码输出
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_connect_ex(uint8_t *buf, size_t buf_size, const mqtt_pkt_connect_config_t *config, mqtt_pkt_error_t *error);

/**
 * @brief 构建 MQTT Publish 报文
 * @param buf 缓冲区
 * @param topic 主题
 * @param payload 载荷
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_publish(uint8_t *buf, const char *topic, const char *payload);

/**
 * @brief 使用自定义配置构建 MQTT Publish 报文
 * @param buf 缓冲区
 * @param buf_size 缓冲区大小
 * @param config 配置选项
 * @param error 错误码输出
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_publish_ex(uint8_t *buf, size_t buf_size, const mqtt_pkt_publish_config_t *config, mqtt_pkt_error_t *error);

/**
 * @brief 构建 MQTT Subscribe 报文
 * @param buf 缓冲区
 * @param buf_size 缓冲区大小
 * @param packet_id 报文 ID
 * @param topics 主题列表
 * @param topic_count 主题数量
 * @param error 错误码输出
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_subscribe(uint8_t *buf, size_t buf_size, uint16_t packet_id, const mqtt_pkt_topic_t *topics, size_t topic_count, mqtt_pkt_error_t *error);

/**
 * @brief 构建 MQTT Unsubscribe 报文
 * @param buf 缓冲区
 * @param buf_size 缓冲区大小
 * @param packet_id 报文 ID
 * @param topics 主题列表
 * @param topic_count 主题数量
 * @param error 错误码输出
 * @return 报文长度，失败返回 0
 */
size_t mqtt_pkt_unsubscribe(uint8_t *buf, size_t buf_size, uint16_t packet_id, const char **topics, size_t topic_count, mqtt_pkt_error_t *error);

/**
 * @brief 构建 MQTT PingReq 报文
 * @param buf 缓冲区
 * @return 报文长度
 */
size_t mqtt_pkt_pingreq(uint8_t *buf);

/**
 * @brief 构建 MQTT Disconnect 报文
 * @param buf 缓冲区
 * @return 报文长度
 */
size_t mqtt_pkt_disconnect(uint8_t *buf);

/**
 * @brief 解析 MQTT 报文头部
 * @param buf 缓冲区
 * @param buf_len 缓冲区长度
 * @param type 报文类型输出
 * @param flags 标志输出
 * @param remaining_length 剩余长度输出
 * @param payload_offset 载荷偏移输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool mqtt_pkt_parse_header(const uint8_t *buf, size_t buf_len, mqtt_pkt_type_t *type, uint8_t *flags, size_t *remaining_length, size_t *payload_offset, mqtt_pkt_error_t *error);

/**
 * @brief 解析完整的 MQTT 报文
 * @param buf 缓冲区
 * @param buf_len 缓冲区长度
 * @param result 解析结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool mqtt_pkt_parse(const uint8_t *buf, size_t buf_len, mqtt_pkt_parse_result_t *result, mqtt_pkt_error_t *error);

/**
 * @brief 计算 MQTT 报文的剩余长度编码
 * @param length 剩余长度
 * @param buf 缓冲区
 * @param buf_size 缓冲区大小
 * @param encoded_len 编码长度输出
 * @return 是否成功
 */
bool mqtt_pkt_encode_remaining_length(size_t length, uint8_t *buf, size_t buf_size, size_t *encoded_len);

/**
 * @brief 解码 MQTT 报文的剩余长度
 * @param buf 缓冲区
 * @param buf_len 缓冲区长度
 * @param length 解码后的长度输出
 * @param encoded_len 编码长度输出
 * @return 是否成功
 */
bool mqtt_pkt_decode_remaining_length(const uint8_t *buf, size_t buf_len, size_t *length, size_t *encoded_len);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* mqtt_pkt_error_string(mqtt_pkt_error_t error);

#endif // C_UTILS_MQTT_PKT_H
