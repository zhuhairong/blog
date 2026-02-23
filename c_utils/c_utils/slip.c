#include "slip.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void slip_config_init(slip_config_t *config) {
    if (!config) return;
    memset(config, 0, sizeof(slip_config_t));
    config->add_end_delimiter = true;
    config->add_start_delimiter = false;
    config->enable_crc = false;
    config->enable_escape = true;
    config->max_packet_size = 4096;
    config->buffer_size = 8192;
}

void slip_state_init(slip_state_t *state) {
    if (!state) return;
    memset(state, 0, sizeof(slip_state_t));
    state->is_initialized = true;
}

void slip_reset_state(slip_state_t *state) {
    if (!state) return;
    slip_state_init(state);
}

size_t slip_encode(const unsigned char *in, size_t len, unsigned char *out) {
    if (!in || !out) return 0;
    
    size_t j = 0;
    out[j++] = SLIP_END;
    for (size_t i = 0; i < len; i++) {
        if (in[i] == SLIP_END) {
            out[j++] = SLIP_ESC;
            out[j++] = SLIP_ESC_END;
        } else if (in[i] == SLIP_ESC) {
            out[j++] = SLIP_ESC;
            out[j++] = SLIP_ESC_ESC;
        } else {
            out[j++] = in[i];
        }
    }
    out[j++] = SLIP_END;
    return j;
}

size_t slip_encode_ex(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, const slip_config_t *config, slip_state_t *state) {
    if (!in || !out) {
        if (state) state->last_error = SLIP_ERROR_INVALID_PARAMS;
        return 0;
    }
    
    slip_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        slip_config_init(&cfg);
    }
    
    size_t max_needed = slip_calculate_max_encoded_size(len, &cfg);
    if (out_len < max_needed) {
        if (state) state->last_error = SLIP_ERROR_BUFFER_TOO_SMALL;
        return 0;
    }
    
    size_t j = 0;
    
    if (cfg.add_start_delimiter) {
        out[j++] = SLIP_END;
    }
    
    for (size_t i = 0; i < len; i++) {
        if (cfg.enable_escape) {
            if (in[i] == SLIP_END) {
                out[j++] = SLIP_ESC;
                out[j++] = SLIP_ESC_END;
            } else if (in[i] == SLIP_ESC) {
                out[j++] = SLIP_ESC;
                out[j++] = SLIP_ESC_ESC;
            } else {
                out[j++] = in[i];
            }
        } else {
            out[j++] = in[i];
        }
    }
    
    if (cfg.add_end_delimiter) {
        out[j++] = SLIP_END;
    }
    
    if (state) {
        state->total_encoded += j;
        state->packets_encoded++;
        state->last_error = SLIP_OK;
    }
    
    return j;
}

size_t slip_decode(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, slip_state_t *state) {
    if (!in || !out) {
        if (state) state->last_error = SLIP_ERROR_INVALID_PARAMS;
        return 0;
    }
    
    size_t j = 0;
    size_t i = 0;
    
    if (len > 0 && in[0] == SLIP_END) {
        i = 1;
    }
    
    while (i < len && j < out_len) {
        if (in[i] == SLIP_END) {
            break;
        } else if (in[i] == SLIP_ESC) {
            i++;
            if (i >= len) {
                if (state) state->last_error = SLIP_ERROR_INCOMPLETE_PACKET;
                return 0;
            }
            if (in[i] == SLIP_ESC_END) {
                out[j++] = SLIP_END;
            } else if (in[i] == SLIP_ESC_ESC) {
                out[j++] = SLIP_ESC;
            } else {
                if (state) state->last_error = SLIP_ERROR_INVALID_CHAR;
                return 0;
            }
            i++;
        } else {
            out[j++] = in[i++];
        }
    }
    
    if (state) {
        state->total_decoded += j;
        state->packets_decoded++;
        state->last_error = SLIP_OK;
    }
    
    return j;
}

size_t slip_decode_ex(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, const slip_config_t *config, slip_state_t *state) {
    if (!in || !out) {
        if (state) state->last_error = SLIP_ERROR_INVALID_PARAMS;
        return 0;
    }
    
    slip_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        slip_config_init(&cfg);
    }
    
    size_t j = 0;
    size_t i = 0;
    
    if (len > 0 && in[0] == SLIP_END) {
        i = 1;
    }
    
    while (i < len && j < out_len) {
        if (in[i] == SLIP_END) {
            break;
        } else if (cfg.enable_escape && in[i] == SLIP_ESC) {
            i++;
            if (i >= len) {
                if (state) state->last_error = SLIP_ERROR_INCOMPLETE_PACKET;
                return 0;
            }
            if (in[i] == SLIP_ESC_END) {
                out[j++] = SLIP_END;
            } else if (in[i] == SLIP_ESC_ESC) {
                out[j++] = SLIP_ESC;
            } else {
                if (state) state->last_error = SLIP_ERROR_INVALID_CHAR;
                return 0;
            }
            i++;
        } else {
            out[j++] = in[i++];
        }
    }
    
    if (j > cfg.max_packet_size) {
        if (state) state->last_error = SLIP_ERROR_BUFFER_TOO_SMALL;
        return 0;
    }
    
    if (state) {
        state->total_decoded += j;
        state->packets_decoded++;
        state->current_packet_size = j;
        state->last_error = SLIP_OK;
    }
    
    return j;
}

slip_error_t slip_encode_file(const char *input_file, const char *output_file, const slip_config_t *config, slip_state_t *state) {
    if (!input_file || !output_file) {
        if (state) state->last_error = SLIP_ERROR_INVALID_PARAMS;
        return SLIP_ERROR_INVALID_PARAMS;
    }
    
    FILE *fin = fopen(input_file, "rb");
    if (!fin) {
        if (state) state->last_error = SLIP_ERROR_FILE_OPEN;
        return SLIP_ERROR_FILE_OPEN;
    }
    
    FILE *fout = fopen(output_file, "wb");
    if (!fout) {
        fclose(fin);
        if (state) state->last_error = SLIP_ERROR_FILE_OPEN;
        return SLIP_ERROR_FILE_OPEN;
    }
    
    slip_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        slip_config_init(&cfg);
    }
    
    unsigned char *in_buf = (unsigned char*)malloc(cfg.buffer_size);
    unsigned char *out_buf = (unsigned char*)malloc(cfg.buffer_size * 2 + 4);
    
    if (!in_buf || !out_buf) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (state) state->last_error = SLIP_ERROR_MEMORY;
        return SLIP_ERROR_MEMORY;
    }
    
    size_t bytes_read;
    while ((bytes_read = fread(in_buf, 1, cfg.buffer_size, fin)) > 0) {
        size_t encoded = slip_encode_ex(in_buf, bytes_read, out_buf, cfg.buffer_size * 2 + 4, &cfg, state);
        if (encoded == 0) {
            free(in_buf);
            free(out_buf);
            fclose(fin);
            fclose(fout);
            return state ? state->last_error : SLIP_ERROR_ENCODE;
        }
        
        if (fwrite(out_buf, 1, encoded, fout) != encoded) {
            free(in_buf);
            free(out_buf);
            fclose(fin);
            fclose(fout);
            if (state) state->last_error = SLIP_ERROR_FILE_WRITE;
            return SLIP_ERROR_FILE_WRITE;
        }
    }
    
    free(in_buf);
    free(out_buf);
    fclose(fin);
    fclose(fout);
    
    if (state) state->last_error = SLIP_OK;
    return SLIP_OK;
}

slip_error_t slip_decode_file(const char *input_file, const char *output_file, const slip_config_t *config, slip_state_t *state) {
    if (!input_file || !output_file) {
        if (state) state->last_error = SLIP_ERROR_INVALID_PARAMS;
        return SLIP_ERROR_INVALID_PARAMS;
    }
    
    FILE *fin = fopen(input_file, "rb");
    if (!fin) {
        if (state) state->last_error = SLIP_ERROR_FILE_OPEN;
        return SLIP_ERROR_FILE_OPEN;
    }
    
    FILE *fout = fopen(output_file, "wb");
    if (!fout) {
        fclose(fin);
        if (state) state->last_error = SLIP_ERROR_FILE_OPEN;
        return SLIP_ERROR_FILE_OPEN;
    }
    
    slip_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        slip_config_init(&cfg);
    }
    
    unsigned char *in_buf = (unsigned char*)malloc(cfg.buffer_size);
    unsigned char *out_buf = (unsigned char*)malloc(cfg.buffer_size);
    
    if (!in_buf || !out_buf) {
        free(in_buf);
        free(out_buf);
        fclose(fin);
        fclose(fout);
        if (state) state->last_error = SLIP_ERROR_MEMORY;
        return SLIP_ERROR_MEMORY;
    }
    
    size_t bytes_read;
    while ((bytes_read = fread(in_buf, 1, cfg.buffer_size, fin)) > 0) {
        size_t decoded = slip_decode_ex(in_buf, bytes_read, out_buf, cfg.buffer_size, &cfg, state);
        if (decoded == 0 && state && state->last_error != SLIP_OK) {
            free(in_buf);
            free(out_buf);
            fclose(fin);
            fclose(fout);
            return state->last_error;
        }
        
        if (decoded > 0 && fwrite(out_buf, 1, decoded, fout) != decoded) {
            free(in_buf);
            free(out_buf);
            fclose(fin);
            fclose(fout);
            if (state) state->last_error = SLIP_ERROR_FILE_WRITE;
            return SLIP_ERROR_FILE_WRITE;
        }
    }
    
    free(in_buf);
    free(out_buf);
    fclose(fin);
    fclose(fout);
    
    if (state) state->last_error = SLIP_OK;
    return SLIP_OK;
}

size_t slip_calculate_max_encoded_size(size_t input_len, const slip_config_t *config) {
    slip_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        slip_config_init(&cfg);
    }
    
    size_t max_size = input_len * 2;
    
    if (cfg.add_start_delimiter) {
        max_size++;
    }
    if (cfg.add_end_delimiter) {
        max_size++;
    }
    
    return max_size;
}

const char* slip_strerror(const slip_state_t *state) {
    if (!state) return "Invalid state";
    
    switch (state->last_error) {
        case SLIP_OK: return "Success";
        case SLIP_ERROR_INVALID_PARAMS: return "Invalid parameters";
        case SLIP_ERROR_ENCODE: return "Encoding error";
        case SLIP_ERROR_DECODE: return "Decoding error";
        case SLIP_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case SLIP_ERROR_INVALID_CHAR: return "Invalid character in input";
        case SLIP_ERROR_FILE_OPEN: return "Failed to open file";
        case SLIP_ERROR_FILE_READ: return "Failed to read file";
        case SLIP_ERROR_FILE_WRITE: return "Failed to write file";
        case SLIP_ERROR_MEMORY: return "Memory allocation error";
        case SLIP_ERROR_INCOMPLETE_PACKET: return "Incomplete packet";
        default: return "Unknown error";
    }
}
