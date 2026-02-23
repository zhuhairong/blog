#ifndef C_UTILS_RINGBUF_H
#define C_UTILS_RINGBUF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 环形缓冲区错误码
 */
typedef enum {
    RINGBUF_OK = 0,                  /**< 成功 */
    RINGBUF_ERROR_NULL_PTR,          /**< 空指针错误 */
    RINGBUF_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RINGBUF_ERROR_CAPACITY_TOO_SMALL, /**< 容量过小 */
    RINGBUF_ERROR_OUT_OF_MEMORY,     /**< 内存不足 */
    RINGBUF_ERROR_BUFFER_FULL,       /**< 缓冲区已满 */
    RINGBUF_ERROR_BUFFER_EMPTY,      /**< 缓冲区为空 */
    RINGBUF_ERROR_MAX                /**< 最大错误码 */
} ringbuf_error_t;

/**
 * @brief 环形缓冲区配置
 */
typedef struct {
    size_t capacity;                 /**< 缓冲区容量 */
    bool zero_initialize;            /**< 是否零初始化内存 */
    bool thread_safe;                /**< 是否线程安全 */
    bool overwrite;                  /**< 是否覆盖模式 */
    size_t alignment;                /**< 内存对齐要求 (0 表示默认对齐) */
} ringbuf_config_t;

/**
 * @brief 环形缓冲区状态
 */
typedef struct {
    size_t capacity;                 /**< 总容量 */
    size_t size;                     /**< 当前使用大小 */
    size_t available;                /**< 可用空间 */
    size_t read_pos;                 /**< 读取位置 */
    size_t write_pos;                /**< 写入位置 */
    bool is_full;                    /**< 是否已满 */
    bool is_empty;                   /**< 是否为空 */
    bool is_overwriting;             /**< 是否正在覆盖 */
} ringbuf_state_t;

/**
 * @brief 环形缓冲区结构
 */
typedef struct ringbuf_s ringbuf_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
ringbuf_config_t ringbuf_default_config(void);

/**
 * @brief 创建环形缓冲区
 * @param capacity 缓冲区容量
 * @return 环形缓冲区指针，失败返回 NULL
 */
ringbuf_t* ringbuf_create(size_t capacity);

/**
 * @brief 使用自定义配置创建环形缓冲区
 * @param config 配置选项
 * @param error 错误码输出
 * @return 环形缓冲区指针，失败返回 NULL
 */
ringbuf_t* ringbuf_create_ex(const ringbuf_config_t *config, ringbuf_error_t *error);

/**
 * @brief 销毁环形缓冲区
 * @param rb 环形缓冲区指针
 */
void ringbuf_free(ringbuf_t *rb);

/**
 * @brief 销毁环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuf_free_ex(ringbuf_t *rb, ringbuf_error_t *error);

/**
 * @brief 写入数据到环形缓冲区
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际写入的字节数
 */
size_t ringbuf_write(ringbuf_t *rb, const uint8_t *data, size_t len);

/**
 * @brief 写入数据到环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际写入的字节数
 */
size_t ringbuf_write_ex(ringbuf_t *rb, const uint8_t *data, size_t len, ringbuf_error_t *error);

/**
 * @brief 从环形缓冲区读取数据
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际读取的字节数
 */
size_t ringbuf_read(ringbuf_t *rb, uint8_t *data, size_t len);

/**
 * @brief 从环形缓冲区读取数据（带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际读取的字节数
 */
size_t ringbuf_read_ex(ringbuf_t *rb, uint8_t *data, size_t len, ringbuf_error_t *error);

/**
 * @brief 查看环形缓冲区数据（不消费）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 实际查看的字节数
 */
size_t ringbuf_peek(const ringbuf_t *rb, uint8_t *data, size_t len);

/**
 * @brief 查看环形缓冲区数据（不消费，带错误处理）
 * @param rb 环形缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 实际查看的字节数
 */
size_t ringbuf_peek_ex(const ringbuf_t *rb, uint8_t *data, size_t len, ringbuf_error_t *error);

/**
 * @brief 获取环形缓冲区当前使用大小
 * @param rb 环形缓冲区指针
 * @return 当前使用大小
 */
size_t ringbuf_size(const ringbuf_t *rb);

/**
 * @brief 获取环形缓冲区可用空间
 * @param rb 环形缓冲区指针
 * @return 可用空间
 */
size_t ringbuf_avail(const ringbuf_t *rb);

/**
 * @brief 检查环形缓冲区是否已满
 * @param rb 环形缓冲区指针
 * @return 是否已满
 */
bool ringbuf_is_full(const ringbuf_t *rb);

/**
 * @brief 检查环形缓冲区是否为空
 * @param rb 环形缓冲区指针
 * @return 是否为空
 */
bool ringbuf_is_empty(const ringbuf_t *rb);

/**
 * @brief 清空环形缓冲区
 * @param rb 环形缓冲区指针
 * @return 是否成功
 */
bool ringbuf_clear(ringbuf_t *rb);

/**
 * @brief 清空环形缓冲区（带错误处理）
 * @param rb 环形缓冲区指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuf_clear_ex(ringbuf_t *rb, ringbuf_error_t *error);

/**
 * @brief 获取环形缓冲区状态
 * @param rb 环形缓冲区指针
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuf_get_state(const ringbuf_t *rb, ringbuf_state_t *state, ringbuf_error_t *error);

/**
 * @brief 调整环形缓冲区大小
 * @param rb 环形缓冲区指针
 * @param new_capacity 新容量
 * @param error 错误码输出
 * @return 是否成功
 */
bool ringbuf_resize(ringbuf_t *rb, size_t new_capacity, ringbuf_error_t *error);

/**
 * @brief 跳过指定数量的字节
 * @param rb 环形缓冲区指针
 * @param len 跳过的字节数
 * @param error 错误码输出
 * @return 实际跳过的字节数
 */
size_t ringbuf_skip(ringbuf_t *rb, size_t len, ringbuf_error_t *error);

/**
 * @brief 从环形缓冲区复制数据到另一个环形缓冲区
 * @param src 源环形缓冲区
 * @param dst 目标环形缓冲区
 * @param len 复制的字节数
 * @param error 错误码输出
 * @return 实际复制的字节数
 */
size_t ringbuf_copy(ringbuf_t *src, ringbuf_t *dst, size_t len, ringbuf_error_t *error);

/**
 * @brief 查找指定字节在环形缓冲区中的位置
 * @param rb 环形缓冲区指针
 * @param byte 要查找的字节
 * @param start_pos 起始位置
 * @param error 错误码输出
 * @return 找到的位置，未找到返回 -1
 */
size_t ringbuf_find(ringbuf_t *rb, uint8_t byte, size_t start_pos, ringbuf_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* ringbuf_error_string(ringbuf_error_t error);

#endif // C_UTILS_RINGBUF_H
