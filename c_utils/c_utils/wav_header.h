#ifndef C_UTILS_WAV_HEADER_H
#define C_UTILS_WAV_HEADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// WAV 头错误码
typedef enum {
    WAV_HDR_OK = 0,
    WAV_HDR_INVALID_PARAMS = -1,
    WAV_HDR_INVALID_FORMAT = -2,
    WAV_HDR_MEMORY_ERROR = -3,
    WAV_HDR_FILE_ERROR = -4,
    WAV_HDR_UNSUPPORTED_FORMAT = -5
} wav_hdr_error_t;

// WAV 头配置选项
typedef struct {
    bool strict_validation;     // 是否使用严格验证
    bool allow_non_pcm;         // 是否允许非PCM格式
    bool check_file_size;       // 是否检查文件大小
    size_t max_header_size;     // 最大头大小
} wav_hdr_config_t;

// WAV 头结构体
typedef struct {
    char chunk_id[4]; uint32_t chunk_size; char format[4];
    char subchunk1_id[4]; uint32_t subchunk1_size; uint16_t audio_format;
    uint16_t num_channels; uint32_t sample_rate; uint32_t byte_rate;
    uint16_t block_align; uint16_t bits_per_sample;
    char subchunk2_id[4]; uint32_t subchunk2_size;
} wav_hdr_t;

// WAV 头上下文
typedef struct {
    wav_hdr_config_t config;
    wav_hdr_error_t last_error;
    size_t init_count;
    size_t validate_count;
} wav_hdr_ctx_t;

// 创建 WAV 头上下文
wav_hdr_error_t wav_hdr_create(wav_hdr_ctx_t** ctx, const wav_hdr_config_t* config);

// 销毁 WAV 头上下文
void wav_hdr_destroy(wav_hdr_ctx_t* ctx);

// 初始化 WAV 头
// 返回 WAV_HDR_OK 表示成功，其他值表示错误
wav_hdr_error_t wav_hdr_init_safe(wav_hdr_ctx_t* ctx, wav_hdr_t *hdr, uint32_t sample_rate, uint16_t channels, uint16_t bits, uint32_t data_len);

// 验证 WAV 头
// 返回 WAV_HDR_OK 表示成功，其他值表示错误
wav_hdr_error_t wav_hdr_validate(wav_hdr_ctx_t* ctx, const wav_hdr_t* hdr);

// 从文件读取 WAV 头
// 返回 WAV_HDR_OK 表示成功，其他值表示错误
wav_hdr_error_t wav_hdr_read(wav_hdr_ctx_t* ctx, const char* filename, wav_hdr_t* hdr);

// 写入 WAV 头到文件
// 返回 WAV_HDR_OK 表示成功，其他值表示错误
wav_hdr_error_t wav_hdr_write(wav_hdr_ctx_t* ctx, const char* filename, const wav_hdr_t* hdr);

// 获取音频数据大小
// 返回音频数据的字节数
uint32_t wav_hdr_get_data_size(const wav_hdr_t* hdr);

// 设置音频数据大小
// 返回 WAV_HDR_OK 表示成功，其他值表示错误
wav_hdr_error_t wav_hdr_set_data_size(wav_hdr_ctx_t* ctx, wav_hdr_t* hdr, uint32_t data_size);

// 获取总文件大小
// 返回总文件大小（包括头）
uint32_t wav_hdr_get_file_size(const wav_hdr_t* hdr);

// 检查是否为 PCM 格式
// 返回 true 表示是 PCM 格式，false 表示不是
bool wav_hdr_is_pcm(const wav_hdr_t* hdr);

// 获取音频格式名称
// 返回格式名称字符串
const char* wav_hdr_get_format_name(uint16_t format);

// 传统初始化函数（向后兼容）
void wav_hdr_init(wav_hdr_t *hdr, uint32_t sample_rate, uint16_t channels, uint16_t bits, uint32_t data_len);

// 获取最后一次错误
wav_hdr_error_t wav_hdr_get_last_error(wav_hdr_ctx_t* ctx);

// 获取错误信息
const char* wav_hdr_strerror(wav_hdr_error_t error);

#endif // C_UTILS_WAV_HEADER_H
