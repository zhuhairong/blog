#ifndef C_UTILS_RINGBUFFER_H
#define C_UTILS_RINGBUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 环形缓冲区错误码
 */
typedef enum {
    RINGBUFFER_OK = 0,                  /**< 成功 */
    RINGBUFFER_ERROR_NULL_PTR,          /**< 空指针错误 */
    RINGBUFFER_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RINGBUFFER_ERROR_SIZE_TOO_SMALL,    /**< 大小过小 */
    RINGBUFFER_ERROR_OUT_OF_MEMORY,     /**< 内存不足 */
    RINGBUFFER_ERROR_FULL,              /**< 缓冲区已满 */
    RINGBUFFER_ERROR_EMPTY,             /**< 缓冲区为空 */
    RINGBUFFER_ERROR_MAX                /**< 最大错误码 */
} ringbuffer_error_t;

/**
 * @brief 环形缓冲区配置
 */
typedef struct {
    size_t size;                      /**< 缓冲区大小 */
    bool zero_initialize;            /**< 是否零初始化内存 */
    bool thread_safe;                /**< 是否线程安全 */
    bool overwrite;                  /**< 是否覆盖模式 */
    bool use_malloc;                 /**< 是否使用 malloc 分配内存 */
    size_t alignment;                /**< 内存对齐要求 (0 表示默认对齐) */
} ringbuffer_config_t;

/**
 * @brief 环形缓冲区状态
 */
typedef struct {
    size_t size;                     /**< 总大小 */
    size_t used;                     /**< 已使用大小 */
    size_t available_read;           /**< 可读取大小 */
    size_t available_write;          /**< 可写入大小 */
    size_t read_pos;                 /**< 读取位置 */
    size_t write_pos;                /**< 写入位置 */
    bool is_full;                    /**< 是否已满 */
    bool is_empty;                   /**< 是否为空 */
    bool is_overwriting;             /**< 是否正在覆盖 */
} ringbuffer_state_t;

/**
 * @brief 环形缓冲区结构
 */
typedef struct ringbuffer_s ringbuffer_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
ringbuffer_config_t ringbuffer_default_config(void);

/**
 * @brief 创建环形缓冲区
 * @param size 缓冲区大小
 * @return 环形缓冲区指针，失败返回 NULL
 */
ringbuffer_t* ringbuffer_create(size_t size);

/**
 * @brief 使用自定义配置创建环形缓冲区
 * @param config 配置选项
 * @param error 错误码输出
 * @return 环形缓冲区指针，失败返回 NULL
 */
ringbuffer_t* ringbuffer_create_ex(const ringbuffer_config_t *config, ringbuffer_error_t *error);

/**
 * @brief 销毁环形缓冲区
 * @param rb 环形缓冲区指针
 */
void ringbuffer_free(ringbuffer_t *rb);

/**
 * @brief 销毁环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_free_ex(ringbuffer_t *rb, ringbuffer_error_t *error);

/**
 * @brief 写入数据到环形缓冲区
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际写入的字节数
 */
size_t ringbuffer_write(ringbuffer_t *rb, const void *data, size_t len);

/**
 * @brief 写入数据到环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际写入的字节数
 */
size_t ringbuffer_write_ex(ringbuffer_t *rb, const void *data, size_t len, ringbuffer_error_t *error);

/**
 * @brief 从环形缓冲区读取数据
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际读取的字节数
 */
size_t ringbuffer_read(ringbuffer_t *rb, void *data, size_t len);

/**
 * @brief 从环形缓冲区读取数据（带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际读取的字节数
 */
size_t ringbuffer_read_ex(ringbuffer_t *rb, void *data, size_t len, ringbuffer_error_t *error);

/**
 * @brief 查看环形缓冲区数据（不消费）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际查看的字节数
 */
size_t ringbuffer_peek(ringbuffer_t *rb, void *data, size_t len);

/**
 * @brief 查看环形缓冲区数据（不消费，带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际查看的字节数
 */
size_t ringbuffer_peek_ex(ringbuffer_t *rb, void *data, size_t len, ringbuffer_error_t *error);

/**
 * @brief 获取可读取大小
 * @param rb 环形缓冲区指针
 * @return 可读取大小
 */
size_t ringbuffer_available_read(const ringbuffer_t *rb);

/**
 * @brief 获取可写入大小
 * @param rb 环形缓冲区指针
 * @return 可写入大小
 */
size_t ringbuffer_available_write(const ringbuffer_t *rb);

/**
 * @brief 检查环形缓冲区是否已满
 * @param rb 环形缓冲区指针
 * @return 是否已满
 */
bool ringbuffer_is_full(const ringbuffer_t *rb);

/**
 * @brief 检查环形缓冲区是否为空
 * @param rb 环形缓冲区指针
 * @return 是否为空
 */
bool ringbuffer_is_empty(const ringbuffer_t *rb);

/**
 * @brief 清空环形缓冲区
 * @param rb 环形缓冲区指针
 * @return 是否成功
 */
bool ringbuffer_clear(ringbuffer_t *rb);

/**
 * @brief 清空环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_clear_ex(ringbuffer_t *rb, ringbuffer_error_t *error);

/**
 * @brief 获取环形缓冲区状态
 * @param rb 环形缓冲区指针
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_get_state(const ringbuffer_t *rb, ringbuffer_state_t *state, ringbuffer_error_t *error);

/**
 * @brief 调整环形缓冲区大小
 * @param rb 环形缓冲区指针
 * @param new_size 新大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_resize(ringbuffer_t *rb, size_t new_size, ringbuffer_error_t *error);

/**
 * @brief 跳过指定数量的字节
 * @param rb 环形缓冲区指针
 * @param len 跳过的字节数
 * @param error 错误码输出
 * @return 实际跳过的字节数
 */
size_t ringbuffer_skip(ringbuffer_t *rb, size_t len, ringbuffer_error_t *error);

/**
 * @brief 写入单个字节
 * @param rb 环形缓冲区指针
 * @param byte 字节数据
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_write_byte(ringbuffer_t *rb, uint8_t byte, ringbuffer_error_t *error);

/**
 * @brief 读取单个字节
 * @param rb 环形缓冲区指针
 * @param byte 字节数据输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuffer_read_byte(ringbuffer_t *rb, uint8_t *byte, ringbuffer_error_t *error);

/**
 * @brief 从环形缓冲区复制数据到另一个环形缓冲区
 * @param src 源环形缓冲区
 * @param dst 目标环形缓冲区
 * @param len 复制的字节数
 * @param error 错误码输出
 * @return 实际复制的字节数
 */
size_t ringbuffer_copy(ringbuffer_t *src, ringbuffer_t *dst, size_t len, ringbuffer_error_t *error);

/**
 * @brief 查找指定字节在环形缓冲区中的位置
 * @param rb 环形缓冲区指针
 * @param byte 要查找的字节
 * @param start_pos 起始位置
 * @param error 错误码输出
 * @return 找到的位置，未找到返回 -1
 */
size_t ringbuffer_find(ringbuffer_t *rb, uint8_t byte, size_t start_pos, ringbuffer_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* ringbuffer_error_string(ringbuffer_error_t error);

#endif // C_UTILS_RINGBUFFER_H
