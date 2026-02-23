#ifndef C_UTILS_XML_PARSE_H
#define C_UTILS_XML_PARSE_H

#include <stddef.h>
#include <stdbool.h>

// XML 解析错误码
typedef enum {
    XML_PARSE_OK = 0,
    XML_PARSE_INVALID_PARAMS = -1,
    XML_PARSE_INVALID_FORMAT = -2,
    XML_PARSE_MEMORY_ERROR = -3,
    XML_PARSE_BUFFER_TOO_SMALL = -4,
    XML_PARSE_TAG_NOT_FOUND = -5,
    XML_PARSE_NESTING_ERROR = -6,
    XML_PARSE_ATTRIBUTE_ERROR = -7
} xml_parse_error_t;

// XML 解析配置选项
typedef struct {
    bool strict_mode;           // 是否使用严格模式
    bool ignore_whitespace;     // 是否忽略空白字符
    bool ignore_case;           // 是否忽略大小写
    bool validate_tags;         // 是否验证标签
    size_t max_xml_length;      // 最大 XML 长度
    size_t max_tag_length;      // 最大标签长度
    size_t max_attr_length;     // 最大属性长度
} xml_parse_config_t;

// XML 属性
typedef struct {
    char *name;
    char *value;
} xml_attribute_t;

// XML 节点
typedef struct xml_node {
    char *tag;
    char *content;
    xml_attribute_t *attributes;
    size_t attribute_count;
    struct xml_node *parent;
    struct xml_node *first_child;
    struct xml_node *next_sibling;
} xml_node_t;

// XML 解析结果
typedef struct {
    xml_node_t *root;
    xml_parse_error_t last_error;
    size_t node_count;
} xml_parse_result_t;

// XML 解析上下文
typedef struct {
    xml_parse_config_t config;
    xml_parse_error_t last_error;
    size_t parse_count;
    size_t validate_count;
} xml_parse_ctx_t;

// 创建 XML 解析上下文
xml_parse_error_t xml_parse_create(xml_parse_ctx_t** ctx, const xml_parse_config_t* config);

// 销毁 XML 解析上下文
void xml_parse_destroy(xml_parse_ctx_t* ctx);

// 提取 XML 标签内容
// 返回 XML_PARSE_OK 表示成功，其他值表示错误
xml_parse_error_t xml_get_tag_content_safe(xml_parse_ctx_t* ctx, const char *xml, const char *tag, char *out, size_t max_len);

// 解析完整 XML 文档
// 返回 XML_PARSE_OK 表示成功，其他值表示错误
xml_parse_error_t xml_parse_document(xml_parse_ctx_t* ctx, const char *xml, xml_parse_result_t** result);

// 销毁 XML 解析结果
void xml_parse_result_destroy(xml_parse_result_t* result);

// 查找 XML 节点
// 返回找到的节点，NULL 表示未找到
xml_node_t* xml_find_node(xml_parse_ctx_t* ctx, const xml_node_t* root, const char *tag);

// 获取 XML 节点属性
// 返回属性值，NULL 表示未找到
const char* xml_get_attribute(const xml_node_t* node, const char *name);

// 验证 XML 文档
// 返回 XML_PARSE_OK 表示成功，其他值表示错误
xml_parse_error_t xml_validate_document(xml_parse_ctx_t* ctx, const char *xml);

// XML 转义
// 返回 XML_PARSE_OK 表示成功，其他值表示错误
xml_parse_error_t xml_escape(xml_parse_ctx_t* ctx, const char *in, char *out, size_t out_size);

// XML 反转义
// 返回 XML_PARSE_OK 表示成功，其他值表示错误
xml_parse_error_t xml_unescape(xml_parse_ctx_t* ctx, const char *in, char *out, size_t out_size);

// 传统标签内容提取函数（向后兼容）
const char* xml_get_tag_content(const char *xml, const char *tag, char *out, int max_len);

// 获取最后一次错误
xml_parse_error_t xml_parse_get_last_error(xml_parse_ctx_t* ctx);

// 获取错误信息
const char* xml_parse_strerror(xml_parse_error_t error);

#endif // C_UTILS_XML_PARSE_H
