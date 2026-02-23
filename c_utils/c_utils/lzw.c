#include "lzw.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LZW_MAX_CODE 4096
#define LZW_MIN_CODE_SIZE 9
#define LZW_MAX_CODE_SIZE 12

typedef struct {
    unsigned short prefix;
    unsigned char suffix;
} lzw_entry_t;

typedef struct {
    lzw_entry_t *entries;
    size_t size;
    size_t capacity;
    size_t next_code;
    size_t code_size;
    size_t max_code_size;
} lzw_dict_t;

typedef struct {
    unsigned char *buffer;
    size_t bit_pos;
    size_t byte_pos;
    size_t size;
} lzw_bitstream_t;

static lzw_dict_t *lzw_dict_create(size_t capacity, size_t initial_code_size) {
    lzw_dict_t *dict = (lzw_dict_t *)malloc(sizeof(lzw_dict_t));
    if (!dict) {
        return NULL;
    }

    dict->entries = (lzw_entry_t *)calloc(capacity, sizeof(lzw_entry_t));
    if (!dict->entries) {
        free(dict);
        return NULL;
    }

    dict->size = 256;
    dict->capacity = capacity;
    dict->next_code = 256;
    dict->code_size = initial_code_size;
    dict->max_code_size = LZW_MAX_CODE_SIZE;

    return dict;
}

static void lzw_dict_free(lzw_dict_t *dict) {
    if (dict) {
        free(dict->entries);
        free(dict);
    }
}

static void lzw_dict_reset(lzw_dict_t *dict) {
    if (dict) {
        dict->size = 256;
        dict->next_code = 256;
        dict->code_size = LZW_MIN_CODE_SIZE;
    }
}

static bool lzw_dict_add(lzw_dict_t *dict, unsigned short prefix, unsigned char suffix) {
    if (dict->next_code >= dict->capacity) {
        return false;
    }

    dict->entries[dict->next_code].prefix = prefix;
    dict->entries[dict->next_code].suffix = suffix;
    dict->next_code++;
    dict->size++;

    if (dict->next_code >= (1 << dict->code_size) && dict->code_size < dict->max_code_size) {
        dict->code_size++;
    }

    return true;
}

static lzw_entry_t *lzw_dict_get(lzw_dict_t *dict, unsigned short code) {
    if (code >= dict->next_code) {
        return NULL;
    }
    return &dict->entries[code];
}

static void lzw_bitstream_init(lzw_bitstream_t *stream, unsigned char *buffer, size_t size) {
    stream->buffer = buffer;
    stream->bit_pos = 0;
    stream->byte_pos = 0;
    stream->size = size;
}

static bool lzw_bitstream_write(lzw_bitstream_t *stream, unsigned int code, size_t bits) {
    for (size_t i = 0; i < bits; i++) {
        if (stream->byte_pos >= stream->size) {
            return false;
        }

        unsigned int bit = (code >> (bits - i - 1)) & 1;
        stream->buffer[stream->byte_pos] |= (bit << (7 - stream->bit_pos));
        stream->bit_pos++;

        if (stream->bit_pos == 8) {
            stream->bit_pos = 0;
            stream->byte_pos++;
        }
    }
    return true;
}

static bool lzw_bitstream_read(lzw_bitstream_t *stream, unsigned int *code, size_t bits) {
    *code = 0;
    for (size_t i = 0; i < bits; i++) {
        if (stream->byte_pos >= stream->size) {
            return false;
        }

        unsigned int bit = (stream->buffer[stream->byte_pos] >> (7 - stream->bit_pos)) & 1;
        *code |= (bit << (bits - i - 1));
        stream->bit_pos++;

        if (stream->bit_pos == 8) {
            stream->bit_pos = 0;
            stream->byte_pos++;
        }
    }
    return true;
}

static size_t lzw_calculate_max_output_size(size_t input_len) {
    return input_len + input_len / 10 + 100;
}

static lzw_error_t lzw_internal_encode(const unsigned char *input, size_t input_len, unsigned char *output, 
                                      size_t output_size, size_t *output_len, const lzw_config_t *config) {
    lzw_dict_t *dict = lzw_dict_create(config->max_dictionary_size, config->initial_code_size);
    if (!dict) {
        return LZW_MEMORY_ERROR;
    }

    lzw_bitstream_t stream;
    lzw_bitstream_init(&stream, output, output_size);

    if (!lzw_bitstream_write(&stream, 256, config->initial_code_size)) {
        lzw_dict_free(dict);
        return LZW_BUFFER_TOO_SMALL;
    }

    unsigned short current_code = input[0];
    for (size_t i = 1; i < input_len; i++) {
        unsigned char next_char = input[i];
        bool found = false;

        for (size_t j = 256; j < dict->next_code; j++) {
            if (dict->entries[j].prefix == current_code && dict->entries[j].suffix == next_char) {
                current_code = (unsigned short)j;
                found = true;
                break;
            }
        }

        if (!found) {
            if (!lzw_bitstream_write(&stream, current_code, dict->code_size)) {
                lzw_dict_free(dict);
                return LZW_BUFFER_TOO_SMALL;
            }

            if (!lzw_dict_add(dict, current_code, next_char)) {
                lzw_dict_reset(dict);
            }

            current_code = next_char;
        }
    }

    if (!lzw_bitstream_write(&stream, current_code, dict->code_size)) {
        lzw_dict_free(dict);
        return LZW_BUFFER_TOO_SMALL;
    }

    if (!lzw_bitstream_write(&stream, 257, dict->code_size)) {
        lzw_dict_free(dict);
        return LZW_BUFFER_TOO_SMALL;
    }

    if (stream.bit_pos > 0) {
        stream.byte_pos++;
    }

    *output_len = stream.byte_pos;
    lzw_dict_free(dict);
    return LZW_OK;
}

static lzw_error_t lzw_internal_decode(const unsigned char *input, size_t input_len, unsigned char *output, 
                                      size_t output_size, size_t *output_len, const lzw_config_t *config) {
    lzw_dict_t *dict = lzw_dict_create(config->max_dictionary_size, config->initial_code_size);
    if (!dict) {
        return LZW_MEMORY_ERROR;
    }

    lzw_bitstream_t stream;
    lzw_bitstream_init(&stream, (unsigned char *)input, input_len);

    unsigned int code;
    if (!lzw_bitstream_read(&stream, &code, config->initial_code_size)) {
        lzw_dict_free(dict);
        return LZW_DECODE_ERROR;
    }

    if (code != 256) {
        lzw_dict_free(dict);
        return LZW_INVALID_CODE;
    }

    size_t out_pos = 0;
    if (!lzw_bitstream_read(&stream, &code, config->initial_code_size)) {
        lzw_dict_free(dict);
        return LZW_DECODE_ERROR;
    }

    output[out_pos++] = (unsigned char)code;
    unsigned short prev_code = (unsigned short)code;

    while (out_pos < output_size) {
        if (!lzw_bitstream_read(&stream, &code, dict->code_size)) {
            break;
        }

        if (code == 257) {
            break;
        }

        if (code < 256) {
            output[out_pos++] = (unsigned char)code;
            lzw_dict_add(dict, prev_code, (unsigned char)code);
            prev_code = (unsigned short)code;
        } else if (code < dict->next_code) {
            lzw_entry_t *entry = &dict->entries[code];
            unsigned char stack[256];
            size_t stack_pos = 0;

            unsigned short curr_code = code;
            while (curr_code >= 256) {
                stack[stack_pos++] = dict->entries[curr_code].suffix;
                curr_code = dict->entries[curr_code].prefix;
            }
            stack[stack_pos++] = (unsigned char)curr_code;

            for (size_t i = stack_pos - 1; i < stack_pos; i--) {
                if (out_pos >= output_size) {
                    lzw_dict_free(dict);
                    return LZW_BUFFER_TOO_SMALL;
                }
                output[out_pos++] = stack[i];
            }

            lzw_dict_add(dict, prev_code, stack[stack_pos - 1]);
            prev_code = (unsigned short)code;
        } else {
            lzw_dict_free(dict);
            return LZW_INVALID_CODE;
        }

        if (dict->next_code >= (1 << dict->code_size) && dict->code_size < dict->max_code_size) {
            dict->code_size++;
        }
    }

    *output_len = out_pos;
    lzw_dict_free(dict);
    return LZW_OK;
}

size_t lzw_encode(const unsigned char *in, size_t in_len, unsigned char *out) {
    lzw_config_t config;
    lzw_get_default_config(&config);
    size_t output_len;
    lzw_error_t error = lzw_encode_ex(in, in_len, out, lzw_calculate_max_output_size(in_len), &output_len, &config);
    return error == LZW_OK ? output_len : 0;
}

lzw_error_t lzw_encode_ex(const unsigned char *input, size_t input_len, unsigned char *output, 
                         size_t output_size, size_t *output_len, const lzw_config_t *config) {
    if (!input || !output || !output_len || !config) {
        return LZW_INVALID_INPUT;
    }

    if (input_len == 0) {
        *output_len = 0;
        return LZW_OK;
    }

    if (output_size < lzw_calculate_max_output_size(input_len)) {
        return LZW_BUFFER_TOO_SMALL;
    }

    return lzw_internal_encode(input, input_len, output, output_size, output_len, config);
}

lzw_error_t lzw_decode(const unsigned char *input, size_t input_len, unsigned char *output, 
                      size_t output_size, size_t *output_len) {
    lzw_config_t config;
    lzw_get_default_config(&config);
    return lzw_decode_ex(input, input_len, output, output_size, output_len, &config);
}

lzw_error_t lzw_decode_ex(const unsigned char *input, size_t input_len, unsigned char *output, 
                         size_t output_size, size_t *output_len, const lzw_config_t *config) {
    if (!input || !output || !output_len || !config) {
        return LZW_INVALID_INPUT;
    }

    if (input_len == 0) {
        *output_len = 0;
        return LZW_OK;
    }

    return lzw_internal_decode(input, input_len, output, output_size, output_len, config);
}

void lzw_get_default_config(lzw_config_t *config) {
    if (config) {
        config->max_code_size = LZW_MAX_CODE_SIZE;
        config->initial_code_size = LZW_MIN_CODE_SIZE;
        config->max_dictionary_size = LZW_MAX_CODE;
        config->enable_reset = true;
        config->use_variable_length = true;
        config->enable_stats = false;
    }
}

double lzw_calculate_ratio(size_t input_size, size_t output_size) {
    if (input_size == 0) {
        return 0.0;
    }
    return (double)output_size / (double)input_size;
}
