#include "run_length_limited.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

rll_config_t rll_default_config(void) {
    rll_config_t config = {
        .min_run_length = 2,
        .max_run_length = 127,
        .use_optimized = false,
        .check_input = true,
        .check_output = true,
        .max_input_size = 1024 * 1024,
        .max_output_size = 2 * 1024 * 1024,
        .use_variable_length = false
    };
    return config;
}

rll_config_t rll_1_7_default_config(void) {
    rll_config_t config = {
        .min_run_length = 1,
        .max_run_length = 7,
        .use_optimized = true,
        .check_input = true,
        .check_output = true,
        .max_input_size = 1024 * 1024,
        .max_output_size = 2 * 1024 * 1024,
        .use_variable_length = false
    };
    return config;
}

size_t rll_encode_basic(const unsigned char *in, size_t len, unsigned char *out) {
    if (!in || !out || len == 0) return 0;
    
    size_t j = 0;
    size_t i = 0;
    
    while (i < len) {
        unsigned char current = in[i];
        size_t run_length = 1;
        
        while (i + run_length < len && in[i + run_length] == current && run_length < 127) {
            run_length++;
        }
        
        if (run_length >= 3) {
            out[j++] = (unsigned char)(0x80 | run_length);
            out[j++] = current;
            i += run_length;
        } else {
            for (size_t k = 0; k < run_length; k++) {
                if (current < 0x80) {
                    out[j++] = current;
                } else {
                    out[j++] = 0x81;
                    out[j++] = current;
                }
            }
            i += run_length;
        }
    }
    
    return j;
}

size_t rll_encode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rll_config_t *config, rll_result_t *result, rll_error_t *error) {
    if (!in || !out) {
        if (error) *error = RLL_ERROR_NULL_PTR;
        return 0;
    }
    
    if (in_len == 0) {
        if (result) {
            memset(result, 0, sizeof(rll_result_t));
        }
        if (error) *error = RLL_OK;
        return 0;
    }
    
    rll_config_t cfg = config ? *config : rll_default_config();
    
    if (cfg.check_input && in_len > cfg.max_input_size) {
        if (error) *error = RLL_ERROR_INPUT_TOO_LARGE;
        return 0;
    }
    
    size_t max_out = rll_calculate_max_output_size(in_len, &cfg);
    if (cfg.check_output && out_size < max_out) {
        if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
        return 0;
    }
    
    size_t j = 0;
    size_t i = 0;
    size_t run_count = 0;
    
    while (i < in_len) {
        unsigned char current = in[i];
        size_t run_length = 1;
        
        size_t max_run = cfg.max_run_length;
        if (max_run > 127) max_run = 127;
        
        while (i + run_length < in_len && 
               in[i + run_length] == current && 
               run_length < max_run) {
            run_length++;
        }
        
        if (run_length >= cfg.min_run_length) {
            if (j + 2 > out_size) {
                if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
                return 0;
            }
            out[j++] = (unsigned char)(0x80 | run_length);
            out[j++] = current;
            run_count++;
            i += run_length;
        } else {
            for (size_t k = 0; k < run_length; k++) {
                if (j >= out_size) {
                    if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
                    return 0;
                }
                if (current < 0x80) {
                    out[j++] = current;
                } else {
                    if (j + 2 > out_size) {
                        if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
                        return 0;
                    }
                    out[j++] = 0x81;
                    out[j++] = current;
                }
            }
            i += run_length;
        }
    }
    
    if (result) {
        result->input_size = in_len;
        result->output_size = j;
        result->run_count = run_count;
        result->compression_ratio = in_len > 0 ? (double)j / (double)in_len : 0.0;
        result->compressed = j < in_len;
        result->optimized = cfg.use_optimized;
    }
    
    if (error) *error = RLL_OK;
    return j;
}

size_t rll_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rll_config_t *config, rll_result_t *result, rll_error_t *error) {
    if (!in || !out) {
        if (error) *error = RLL_ERROR_NULL_PTR;
        return 0;
    }
    
    if (in_len == 0) {
        if (result) {
            memset(result, 0, sizeof(rll_result_t));
        }
        if (error) *error = RLL_OK;
        return 0;
    }
    
    rll_config_t cfg = config ? *config : rll_default_config();
    
    size_t j = 0;
    size_t i = 0;
    size_t run_count = 0;
    
    while (i < in_len) {
        unsigned char byte = in[i];
        
        if (byte & 0x80) {
            size_t run_length = byte & 0x7F;
            
            if (run_length < cfg.min_run_length || run_length > cfg.max_run_length) {
                if (error) *error = RLL_ERROR_INVALID_RUN_LENGTH;
                return 0;
            }
            
            i++;
            if (i >= in_len) {
                if (error) *error = RLL_ERROR_DECODING_FAILED;
                return 0;
            }
            
            if (j + run_length > out_size) {
                if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
                return 0;
            }
            
            unsigned char value = in[i++];
            for (size_t k = 0; k < run_length; k++) {
                out[j++] = value;
            }
            run_count++;
        } else {
            if (j >= out_size) {
                if (error) *error = RLL_ERROR_OUTPUT_TOO_SMALL;
                return 0;
            }
            out[j++] = byte;
            i++;
        }
    }
    
    if (result) {
        result->input_size = in_len;
        result->output_size = j;
        result->run_count = run_count;
        result->compression_ratio = j > 0 ? (double)in_len / (double)j : 0.0;
        result->compressed = false;
        result->optimized = cfg.use_optimized;
    }
    
    if (error) *error = RLL_OK;
    return j;
}

size_t rll_1_7_encode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, rll_result_t *result, rll_error_t *error) {
    rll_config_t config = rll_1_7_default_config();
    return rll_encode(in, in_len, out, out_size, &config, result, error);
}

size_t rll_1_7_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, rll_result_t *result, rll_error_t *error) {
    rll_config_t config = rll_1_7_default_config();
    return rll_decode(in, in_len, out, out_size, &config, result, error);
}

size_t rll_calculate_max_output_size(size_t in_len, const rll_config_t *config) {
    rll_config_t cfg = config ? *config : rll_default_config();
    
    if (cfg.use_variable_length) {
        return in_len * 2 + 256;
    }
    
    return in_len * 2 + 128;
}

bool rll_validate_encoded_data(const unsigned char *in, size_t in_len, const rll_config_t *config, rll_error_t *error) {
    if (!in) {
        if (error) *error = RLL_ERROR_NULL_PTR;
        return false;
    }
    
    rll_config_t cfg = config ? *config : rll_default_config();
    
    size_t i = 0;
    while (i < in_len) {
        unsigned char byte = in[i];
        
        if (byte & 0x80) {
            size_t run_length = byte & 0x7F;
            
            if (run_length < cfg.min_run_length || run_length > cfg.max_run_length) {
                if (error) *error = RLL_ERROR_INVALID_RUN_LENGTH;
                return false;
            }
            
            i++;
            if (i >= in_len) {
                if (error) *error = RLL_ERROR_DECODING_FAILED;
                return false;
            }
            i++;
        } else {
            i++;
        }
    }
    
    if (error) *error = RLL_OK;
    return true;
}

bool rll_compress_file(const char *input_filename, const char *output_filename, const rll_config_t *config, rll_result_t *result, rll_error_t *error) {
    if (!input_filename || !output_filename) {
        if (error) *error = RLL_ERROR_NULL_PTR;
        return false;
    }
    
    FILE *fin = fopen(input_filename, "rb");
    if (!fin) {
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    FILE *fout = fopen(output_filename, "wb");
    if (!fout) {
        fclose(fin);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    rll_config_t cfg = config ? *config : rll_default_config();
    
    unsigned char *in_buf = (unsigned char*)malloc(cfg.max_input_size);
    unsigned char *out_buf = (unsigned char*)malloc(cfg.max_output_size);
    
    if (!in_buf || !out_buf) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_NULL_PTR;
        return false;
    }
    
    size_t bytes_read = fread(in_buf, 1, cfg.max_input_size, fin);
    if (bytes_read == 0) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    size_t encoded = rll_encode(in_buf, bytes_read, out_buf, cfg.max_output_size, &cfg, result, error);
    if (encoded == 0) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        return false;
    }
    
    if (fwrite(out_buf, 1, encoded, fout) != encoded) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    free(in_buf);
    free(out_buf);
    fclose(fin);
    fclose(fout);
    
    if (error) *error = RLL_OK;
    return true;
}

bool rll_decompress_file(const char *input_filename, const char *output_filename, const rll_config_t *config, rll_result_t *result, rll_error_t *error) {
    if (!input_filename || !output_filename) {
        if (error) *error = RLL_ERROR_NULL_PTR;
        return false;
    }
    
    FILE *fin = fopen(input_filename, "rb");
    if (!fin) {
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    FILE *fout = fopen(output_filename, "wb");
    if (!fout) {
        fclose(fin);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    rll_config_t cfg = config ? *config : rll_default_config();
    
    unsigned char *in_buf = (unsigned char*)malloc(cfg.max_output_size);
    unsigned char *out_buf = (unsigned char*)malloc(cfg.max_input_size);
    
    if (!in_buf || !out_buf) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_NULL_PTR;
        return false;
    }
    
    size_t bytes_read = fread(in_buf, 1, cfg.max_output_size, fin);
    if (bytes_read == 0) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    size_t decoded = rll_decode(in_buf, bytes_read, out_buf, cfg.max_input_size, &cfg, result, error);
    if (decoded == 0) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        return false;
    }
    
    if (fwrite(out_buf, 1, decoded, fout) != decoded) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (error) *error = RLL_ERROR_DECODING_FAILED;
        return false;
    }
    
    free(in_buf);
    free(out_buf);
    fclose(fin);
    fclose(fout);
    
    if (error) *error = RLL_OK;
    return true;
}

const char* rll_error_string(rll_error_t error) {
    switch (error) {
        case RLL_OK: return "Success";
        case RLL_ERROR_NULL_PTR: return "Null pointer error";
        case RLL_ERROR_INVALID_ARGS: return "Invalid arguments";
        case RLL_ERROR_INPUT_TOO_LARGE: return "Input too large";
        case RLL_ERROR_OUTPUT_TOO_SMALL: return "Output buffer too small";
        case RLL_ERROR_INVALID_RUN_LENGTH: return "Invalid run length";
        case RLL_ERROR_ENCODING_FAILED: return "Encoding failed";
        case RLL_ERROR_DECODING_FAILED: return "Decoding failed";
        case RLL_ERROR_MAX: return "Max error code";
        default: return "Unknown error";
    }
}
