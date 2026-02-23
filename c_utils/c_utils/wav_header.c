#include "wav_header.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

wav_hdr_error_t wav_hdr_create(wav_hdr_ctx_t** ctx, const wav_hdr_config_t* config) {
    if (!ctx) return WAV_HDR_INVALID_PARAMS;
    
    wav_hdr_ctx_t* c = (wav_hdr_ctx_t*)malloc(sizeof(wav_hdr_ctx_t));
    if (!c) return WAV_HDR_MEMORY_ERROR;
    
    if (config) {
        c->config = *config;
    } else {
        memset(&c->config, 0, sizeof(wav_hdr_config_t));
        c->config.strict_validation = true;
        c->config.allow_non_pcm = false;
        c->config.check_file_size = true;
        c->config.max_header_size = 1024;
    }
    
    c->last_error = WAV_HDR_OK;
    c->init_count = 0;
    c->validate_count = 0;
    
    *ctx = c;
    return WAV_HDR_OK;
}

void wav_hdr_destroy(wav_hdr_ctx_t* ctx) {
    if (ctx) {
        free(ctx);
    }
}

wav_hdr_error_t wav_hdr_init_safe(wav_hdr_ctx_t* ctx, wav_hdr_t *hdr, uint32_t sample_rate, uint16_t channels, uint16_t bits, uint32_t data_len) {
    if (!hdr) return WAV_HDR_INVALID_PARAMS;
    
    if (sample_rate == 0 || channels == 0 || bits == 0) {
        if (ctx) ctx->last_error = WAV_HDR_INVALID_PARAMS;
        return WAV_HDR_INVALID_PARAMS;
    }
    
    memcpy(hdr->chunk_id, "RIFF", 4);
    hdr->chunk_size = 36 + data_len;
    memcpy(hdr->format, "WAVE", 4);
    memcpy(hdr->subchunk1_id, "fmt ", 4);
    hdr->subchunk1_size = 16;
    hdr->audio_format = 1;
    hdr->num_channels = channels;
    hdr->sample_rate = sample_rate;
    hdr->byte_rate = sample_rate * channels * bits / 8;
    hdr->block_align = (uint16_t)(channels * bits / 8);
    hdr->bits_per_sample = bits;
    memcpy(hdr->subchunk2_id, "data", 4);
    hdr->subchunk2_size = data_len;
    
    if (ctx) {
        ctx->init_count++;
        ctx->last_error = WAV_HDR_OK;
    }
    
    return WAV_HDR_OK;
}

wav_hdr_error_t wav_hdr_validate(wav_hdr_ctx_t* ctx, const wav_hdr_t* hdr) {
    if (!hdr) return WAV_HDR_INVALID_PARAMS;
    
    if (memcmp(hdr->chunk_id, "RIFF", 4) != 0) {
        if (ctx) ctx->last_error = WAV_HDR_INVALID_FORMAT;
        return WAV_HDR_INVALID_FORMAT;
    }
    
    if (memcmp(hdr->format, "WAVE", 4) != 0) {
        if (ctx) ctx->last_error = WAV_HDR_INVALID_FORMAT;
        return WAV_HDR_INVALID_FORMAT;
    }
    
    if (memcmp(hdr->subchunk1_id, "fmt ", 4) != 0) {
        if (ctx) ctx->last_error = WAV_HDR_INVALID_FORMAT;
        return WAV_HDR_INVALID_FORMAT;
    }
    
    if (memcmp(hdr->subchunk2_id, "data", 4) != 0) {
        if (ctx) ctx->last_error = WAV_HDR_INVALID_FORMAT;
        return WAV_HDR_INVALID_FORMAT;
    }
    
    if (ctx && ctx->config.strict_validation) {
        if (hdr->audio_format != 1 && !ctx->config.allow_non_pcm) {
            ctx->last_error = WAV_HDR_UNSUPPORTED_FORMAT;
            return WAV_HDR_UNSUPPORTED_FORMAT;
        }
        
        if (hdr->sample_rate == 0 || hdr->num_channels == 0 || hdr->bits_per_sample == 0) {
            ctx->last_error = WAV_HDR_INVALID_FORMAT;
            return WAV_HDR_INVALID_FORMAT;
        }
    }
    
    if (ctx) {
        ctx->validate_count++;
        ctx->last_error = WAV_HDR_OK;
    }
    
    return WAV_HDR_OK;
}

wav_hdr_error_t wav_hdr_read(wav_hdr_ctx_t* ctx, const char* filename, wav_hdr_t* hdr) {
    if (!filename || !hdr) return WAV_HDR_INVALID_PARAMS;
    
    FILE* f = fopen(filename, "rb");
    if (!f) {
        if (ctx) ctx->last_error = WAV_HDR_FILE_ERROR;
        return WAV_HDR_FILE_ERROR;
    }
    
    if (fread(hdr, sizeof(wav_hdr_t), 1, f) != 1) {
        fclose(f);
        if (ctx) ctx->last_error = WAV_HDR_FILE_ERROR;
        return WAV_HDR_FILE_ERROR;
    }
    
    fclose(f);
    
    return wav_hdr_validate(ctx, hdr);
}

wav_hdr_error_t wav_hdr_write(wav_hdr_ctx_t* ctx, const char* filename, const wav_hdr_t* hdr) {
    if (!filename || !hdr) return WAV_HDR_INVALID_PARAMS;
    
    wav_hdr_error_t err = wav_hdr_validate(ctx, hdr);
    if (err != WAV_HDR_OK) return err;
    
    FILE* f = fopen(filename, "wb");
    if (!f) {
        if (ctx) ctx->last_error = WAV_HDR_FILE_ERROR;
        return WAV_HDR_FILE_ERROR;
    }
    
    if (fwrite(hdr, sizeof(wav_hdr_t), 1, f) != 1) {
        fclose(f);
        if (ctx) ctx->last_error = WAV_HDR_FILE_ERROR;
        return WAV_HDR_FILE_ERROR;
    }
    
    fclose(f);
    
    if (ctx) ctx->last_error = WAV_HDR_OK;
    return WAV_HDR_OK;
}

uint32_t wav_hdr_get_data_size(const wav_hdr_t* hdr) {
    if (!hdr) return 0;
    return hdr->subchunk2_size;
}

wav_hdr_error_t wav_hdr_set_data_size(wav_hdr_ctx_t* ctx, wav_hdr_t* hdr, uint32_t data_size) {
    if (!hdr) return WAV_HDR_INVALID_PARAMS;
    
    hdr->subchunk2_size = data_size;
    hdr->chunk_size = 36 + data_size;
    
    if (ctx) ctx->last_error = WAV_HDR_OK;
    return WAV_HDR_OK;
}

uint32_t wav_hdr_get_file_size(const wav_hdr_t* hdr) {
    if (!hdr) return 0;
    return hdr->chunk_size + 8;
}

bool wav_hdr_is_pcm(const wav_hdr_t* hdr) {
    if (!hdr) return false;
    return hdr->audio_format == 1;
}

const char* wav_hdr_get_format_name(uint16_t format) {
    switch (format) {
        case 1: return "PCM";
        case 2: return "Microsoft ADPCM";
        case 3: return "IEEE Float";
        case 6: return "ITU G.711 A-law";
        case 7: return "ITU G.711 µ-law";
        case 17: return "IMA ADPCM";
        case 20: return "ITU G.723 ADPCM (Yamaha)";
        case 49: return "GSM 6.10";
        case 64: return "ITU G.721 ADPCM";
        case 80: return "MPEG";
        case 65534: return "WAVE_FORMAT_EXTENSIBLE";
        default: return "Unknown";
    }
}

void wav_hdr_init(wav_hdr_t *hdr, uint32_t sample_rate, uint16_t channels, uint16_t bits, uint32_t data_len) {
    if (!hdr) return;
    
    memcpy(hdr->chunk_id, "RIFF", 4);
    hdr->chunk_size = 36 + data_len;
    memcpy(hdr->format, "WAVE", 4);
    memcpy(hdr->subchunk1_id, "fmt ", 4);
    hdr->subchunk1_size = 16;
    hdr->audio_format = 1;
    hdr->num_channels = channels;
    hdr->sample_rate = sample_rate;
    hdr->byte_rate = sample_rate * channels * bits / 8;
    hdr->block_align = (uint16_t)(channels * bits / 8);
    hdr->bits_per_sample = bits;
    memcpy(hdr->subchunk2_id, "data", 4);
    hdr->subchunk2_size = data_len;
}

wav_hdr_error_t wav_hdr_get_last_error(wav_hdr_ctx_t* ctx) {
    if (!ctx) return WAV_HDR_INVALID_PARAMS;
    return ctx->last_error;
}

const char* wav_hdr_strerror(wav_hdr_error_t error) {
    switch (error) {
        case WAV_HDR_OK: return "Success";
        case WAV_HDR_INVALID_PARAMS: return "Invalid parameters";
        case WAV_HDR_INVALID_FORMAT: return "Invalid WAV format";
        case WAV_HDR_MEMORY_ERROR: return "Memory allocation error";
        case WAV_HDR_FILE_ERROR: return "File I/O error";
        case WAV_HDR_UNSUPPORTED_FORMAT: return "Unsupported audio format";
        default: return "Unknown error";
    }
}
