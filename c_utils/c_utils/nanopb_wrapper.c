#include "nanopb_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WIRETYPE_VARINT 0
#define WIRETYPE_FIXED64 1
#define WIRETYPE_LENGTH_DELIMITED 2
#define WIRETYPE_FIXED32 5

static uint8_t get_wire_type(npb_type_t type) {
    switch (type) {
        case NPB_TYPE_INT32:
        case NPB_TYPE_INT64:
        case NPB_TYPE_UINT32:
        case NPB_TYPE_UINT64:
        case NPB_TYPE_SINT32:
        case NPB_TYPE_SINT64:
        case NPB_TYPE_BOOL:
        case NPB_TYPE_ENUM:
            return WIRETYPE_VARINT;
        case NPB_TYPE_FIXED64:
        case NPB_TYPE_SFIXED64:
        case NPB_TYPE_DOUBLE:
            return WIRETYPE_FIXED64;
        case NPB_TYPE_FIXED32:
        case NPB_TYPE_SFIXED32:
        case NPB_TYPE_FLOAT:
            return WIRETYPE_FIXED32;
        case NPB_TYPE_STRING:
        case NPB_TYPE_BYTES:
            return WIRETYPE_LENGTH_DELIMITED;
        default:
            return WIRETYPE_VARINT;
    }
}

const char* npb_error_string(npb_error_t err) {
    switch (err) {
        case NPB_OK: return "Success";
        case NPB_ERR_NULL_PTR: return "Null pointer";
        case NPB_ERR_BUFFER_TOO_SMALL: return "Buffer too small";
        case NPB_ERR_INVALID_TYPE: return "Invalid type";
        case NPB_ERR_INVALID_FIELD: return "Invalid field";
        case NPB_ERR_ENCODE_FAILED: return "Encode failed";
        case NPB_ERR_DECODE_FAILED: return "Decode failed";
        case NPB_ERR_OUT_OF_MEMORY: return "Out of memory";
        case NPB_ERR_OVERFLOW: return "Overflow";
        case NPB_ERR_UNDERFLOW: return "Underflow";
        case NPB_ERR_INVALID_TAG: return "Invalid tag";
        default: return "Unknown error";
    }
}

npb_encoder_t* npb_encoder_create(void) {
    npb_encoder_t* enc = (npb_encoder_t*)calloc(1, sizeof(npb_encoder_t));
    if (!enc) return NULL;
    
    enc->buffer = (uint8_t*)malloc(NPB_MAX_MESSAGE_SIZE);
    if (!enc->buffer) {
        free(enc);
        return NULL;
    }
    
    enc->capacity = NPB_MAX_MESSAGE_SIZE;
    enc->pos = 0;
    enc->last_error = NPB_OK;
    enc->owns_buffer = true;
    
    return enc;
}

npb_encoder_t* npb_encoder_create_with_buffer(uint8_t* buffer, size_t capacity) {
    if (!buffer || capacity == 0) return NULL;
    
    npb_encoder_t* enc = (npb_encoder_t*)calloc(1, sizeof(npb_encoder_t));
    if (!enc) return NULL;
    
    enc->buffer = buffer;
    enc->capacity = capacity;
    enc->pos = 0;
    enc->last_error = NPB_OK;
    enc->owns_buffer = false;
    
    return enc;
}

void npb_encoder_destroy(npb_encoder_t* enc) {
    if (enc) {
        if (enc->owns_buffer && enc->buffer) {
            free(enc->buffer);
        }
        free(enc);
    }
}

void npb_encoder_reset(npb_encoder_t* enc) {
    if (enc) {
        enc->pos = 0;
        enc->last_error = NPB_OK;
    }
}

const uint8_t* npb_encoder_buffer(const npb_encoder_t* enc, size_t* size) {
    if (!enc) {
        if (size) *size = 0;
        return NULL;
    }
    if (size) *size = enc->pos;
    return enc->buffer;
}

npb_error_t npb_encoder_last_error(const npb_encoder_t* enc) {
    return enc ? enc->last_error : NPB_ERR_NULL_PTR;
}

npb_decoder_t* npb_decoder_create(const uint8_t* data, size_t size) {
    if (!data || size == 0) return NULL;
    
    npb_decoder_t* dec = (npb_decoder_t*)calloc(1, sizeof(npb_decoder_t));
    if (!dec) return NULL;
    
    dec->buffer = data;
    dec->size = size;
    dec->pos = 0;
    dec->last_error = NPB_OK;
    
    return dec;
}

void npb_decoder_destroy(npb_decoder_t* dec) {
    free(dec);
}

void npb_decoder_reset(npb_decoder_t* dec, const uint8_t* data, size_t size) {
    if (dec && data && size > 0) {
        dec->buffer = data;
        dec->size = size;
        dec->pos = 0;
        dec->last_error = NPB_OK;
    }
}

size_t npb_decoder_remaining(const npb_decoder_t* dec) {
    return dec ? (dec->size - dec->pos) : 0;
}

npb_error_t npb_decoder_last_error(const npb_decoder_t* dec) {
    return dec ? dec->last_error : NPB_ERR_NULL_PTR;
}

size_t npb_zigzag_encode(int32_t value) {
    return (uint32_t)((value << 1) ^ (value >> 31));
}

int32_t npb_zigzag_decode(uint32_t value) {
    return (int32_t)((value >> 1) ^ (-(int32_t)(value & 1)));
}

size_t npb_zigzag_encode64(int64_t value) {
    return (uint64_t)((value << 1) ^ (value >> 63));
}

int64_t npb_zigzag_decode64(uint64_t value) {
    return (int64_t)((value >> 1) ^ (-(int64_t)(value & 1)));
}

size_t npb_varint_size(uint64_t value) {
    size_t size = 1;
    while (value >= 0x80) {
        value >>= 7;
        size++;
    }
    return size;
}

npb_error_t npb_encode_varint(npb_encoder_t* enc, uint64_t value) {
    if (!enc) return NPB_ERR_NULL_PTR;
    
    do {
        if (enc->pos >= enc->capacity) {
            enc->last_error = NPB_ERR_BUFFER_TOO_SMALL;
            return NPB_ERR_BUFFER_TOO_SMALL;
        }
        
        uint8_t byte = (uint8_t)(value & 0x7F);
        value >>= 7;
        if (value != 0) byte |= 0x80;
        enc->buffer[enc->pos++] = byte;
    } while (value != 0);
    
    return NPB_OK;
}

npb_error_t npb_encode_signed_varint(npb_encoder_t* enc, int64_t value) {
    return npb_encode_varint(enc, (uint64_t)value);
}

npb_error_t npb_encode_fixed32(npb_encoder_t* enc, uint32_t value) {
    if (!enc) return NPB_ERR_NULL_PTR;
    if (enc->pos + 4 > enc->capacity) return NPB_ERR_BUFFER_TOO_SMALL;
    
    enc->buffer[enc->pos++] = (uint8_t)(value & 0xFF);
    enc->buffer[enc->pos++] = (uint8_t)((value >> 8) & 0xFF);
    enc->buffer[enc->pos++] = (uint8_t)((value >> 16) & 0xFF);
    enc->buffer[enc->pos++] = (uint8_t)((value >> 24) & 0xFF);
    
    return NPB_OK;
}

npb_error_t npb_encode_fixed64(npb_encoder_t* enc, uint64_t value) {
    if (!enc) return NPB_ERR_NULL_PTR;
    if (enc->pos + 8 > enc->capacity) return NPB_ERR_BUFFER_TOO_SMALL;
    
    for (int i = 0; i < 8; i++) {
        enc->buffer[enc->pos++] = (uint8_t)((value >> (i * 8)) & 0xFF);
    }
    
    return NPB_OK;
}

npb_error_t npb_encode_float(npb_encoder_t* enc, float value) {
    union { float f; uint32_t u; } conv;
    conv.f = value;
    return npb_encode_fixed32(enc, conv.u);
}

npb_error_t npb_encode_double(npb_encoder_t* enc, double value) {
    union { double d; uint64_t u; } conv;
    conv.d = value;
    return npb_encode_fixed64(enc, conv.u);
}

npb_error_t npb_encode_string(npb_encoder_t* enc, const char* str) {
    if (!enc || !str) return NPB_ERR_NULL_PTR;
    
    size_t len = strlen(str);
    npb_error_t err = npb_encode_varint(enc, len);
    if (err != NPB_OK) return err;
    
    if (enc->pos + len > enc->capacity) return NPB_ERR_BUFFER_TOO_SMALL;
    
    memcpy(enc->buffer + enc->pos, str, len);
    enc->pos += len;
    
    return NPB_OK;
}

npb_error_t npb_encode_bytes(npb_encoder_t* enc, const uint8_t* data, size_t len) {
    if (!enc) return NPB_ERR_NULL_PTR;
    if (len > 0 && !data) return NPB_ERR_NULL_PTR;
    
    npb_error_t err = npb_encode_varint(enc, len);
    if (err != NPB_OK) return err;
    
    if (enc->pos + len > enc->capacity) return NPB_ERR_BUFFER_TOO_SMALL;
    
    if (len > 0) {
        memcpy(enc->buffer + enc->pos, data, len);
        enc->pos += len;
    }
    
    return NPB_OK;
}

npb_error_t npb_encode_tag(npb_encoder_t* enc, uint32_t field_id, uint8_t wire_type) {
    if (!enc) return NPB_ERR_NULL_PTR;
    return npb_encode_varint(enc, (field_id << 3) | wire_type);
}

npb_error_t npb_decode_varint(npb_decoder_t* dec, uint64_t* value) {
    if (!dec || !value) return NPB_ERR_NULL_PTR;
    
    *value = 0;
    uint32_t shift = 0;
    
    while (dec->pos < dec->size) {
        uint8_t byte = dec->buffer[dec->pos++];
        *value |= (uint64_t)(byte & 0x7F) << shift;
        shift += 7;
        
        if (!(byte & 0x80)) return NPB_OK;
        if (shift >= 64) {
            dec->last_error = NPB_ERR_OVERFLOW;
            return NPB_ERR_OVERFLOW;
        }
    }
    
    dec->last_error = NPB_ERR_UNDERFLOW;
    return NPB_ERR_UNDERFLOW;
}

npb_error_t npb_decode_signed_varint(npb_decoder_t* dec, int64_t* value) {
    uint64_t uvalue;
    npb_error_t err = npb_decode_varint(dec, &uvalue);
    if (err != NPB_OK) return err;
    *value = (int64_t)uvalue;
    return NPB_OK;
}

npb_error_t npb_decode_fixed32(npb_decoder_t* dec, uint32_t* value) {
    if (!dec || !value) return NPB_ERR_NULL_PTR;
    if (dec->pos + 4 > dec->size) return NPB_ERR_UNDERFLOW;
    
    *value = 0;
    for (int i = 0; i < 4; i++) {
        *value |= (uint32_t)dec->buffer[dec->pos++] << (i * 8);
    }
    
    return NPB_OK;
}

npb_error_t npb_decode_fixed64(npb_decoder_t* dec, uint64_t* value) {
    if (!dec || !value) return NPB_ERR_NULL_PTR;
    if (dec->pos + 8 > dec->size) return NPB_ERR_UNDERFLOW;
    
    *value = 0;
    for (int i = 0; i < 8; i++) {
        *value |= (uint64_t)dec->buffer[dec->pos++] << (i * 8);
    }
    
    return NPB_OK;
}

npb_error_t npb_decode_float(npb_decoder_t* dec, float* value) {
    union { float f; uint32_t u; } conv;
    npb_error_t err = npb_decode_fixed32(dec, &conv.u);
    if (err != NPB_OK) return err;
    *value = conv.f;
    return NPB_OK;
}

npb_error_t npb_decode_double(npb_decoder_t* dec, double* value) {
    union { double d; uint64_t u; } conv;
    npb_error_t err = npb_decode_fixed64(dec, &conv.u);
    if (err != NPB_OK) return err;
    *value = conv.d;
    return NPB_OK;
}

npb_error_t npb_decode_string(npb_decoder_t* dec, char* str, size_t max_len) {
    if (!dec || !str) return NPB_ERR_NULL_PTR;
    
    uint64_t len;
    npb_error_t err = npb_decode_varint(dec, &len);
    if (err != NPB_OK) return err;
    
    if (len >= max_len) len = max_len - 1;
    if (dec->pos + len > dec->size) return NPB_ERR_UNDERFLOW;
    
    memcpy(str, dec->buffer + dec->pos, len);
    str[len] = '\0';
    dec->pos += len;
    
    return NPB_OK;
}

npb_error_t npb_decode_bytes(npb_decoder_t* dec, uint8_t* data, size_t* len, size_t max_len) {
    if (!dec || !len) return NPB_ERR_NULL_PTR;
    
    uint64_t bytes_len;
    npb_error_t err = npb_decode_varint(dec, &bytes_len);
    if (err != NPB_OK) return err;
    
    if (bytes_len > max_len) bytes_len = max_len;
    if (dec->pos + bytes_len > dec->size) return NPB_ERR_UNDERFLOW;
    
    if (data && bytes_len > 0) {
        memcpy(data, dec->buffer + dec->pos, bytes_len);
    }
    dec->pos += bytes_len;
    *len = bytes_len;
    
    return NPB_OK;
}

npb_error_t npb_decode_tag(npb_decoder_t* dec, uint32_t* field_id, uint8_t* wire_type) {
    if (!dec || !field_id || !wire_type) return NPB_ERR_NULL_PTR;
    
    uint64_t tag;
    npb_error_t err = npb_decode_varint(dec, &tag);
    if (err != NPB_OK) return err;
    
    *wire_type = (uint8_t)(tag & 0x07);
    *field_id = (uint32_t)(tag >> 3);
    
    return NPB_OK;
}

static const npb_field_desc_t* find_field(const npb_msgdesc_t* desc, uint32_t field_id) {
    for (size_t i = 0; i < desc->field_count; i++) {
        if (desc->fields[i].field_id == field_id) {
            return &desc->fields[i];
        }
    }
    return NULL;
}

npb_error_t npb_encode_message(npb_encoder_t* enc, const void* msg, const npb_msgdesc_t* desc) {
    if (!enc || !msg || !desc) return NPB_ERR_NULL_PTR;
    
    for (size_t i = 0; i < desc->field_count; i++) {
        const npb_field_desc_t* field = &desc->fields[i];
        const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
        
        npb_error_t err = npb_encode_tag(enc, field->field_id, get_wire_type(field->type));
        if (err != NPB_OK) return err;
        
        switch (field->type) {
            case NPB_TYPE_INT32:
            case NPB_TYPE_INT64:
            case NPB_TYPE_UINT32:
            case NPB_TYPE_UINT64:
            case NPB_TYPE_ENUM:
                err = npb_encode_varint(enc, *(const uint64_t*)field_ptr);
                break;
            case NPB_TYPE_SINT32:
                err = npb_encode_varint(enc, npb_zigzag_encode(*(const int32_t*)field_ptr));
                break;
            case NPB_TYPE_SINT64:
                err = npb_encode_varint(enc, npb_zigzag_encode64(*(const int64_t*)field_ptr));
                break;
            case NPB_TYPE_BOOL:
                err = npb_encode_varint(enc, *(const bool*)field_ptr ? 1 : 0);
                break;
            case NPB_TYPE_FIXED32:
            case NPB_TYPE_SFIXED32:
                err = npb_encode_fixed32(enc, *(const uint32_t*)field_ptr);
                break;
            case NPB_TYPE_FIXED64:
            case NPB_TYPE_SFIXED64:
                err = npb_encode_fixed64(enc, *(const uint64_t*)field_ptr);
                break;
            case NPB_TYPE_FLOAT:
                err = npb_encode_float(enc, *(const float*)field_ptr);
                break;
            case NPB_TYPE_DOUBLE:
                err = npb_encode_double(enc, *(const double*)field_ptr);
                break;
            case NPB_TYPE_STRING:
                err = npb_encode_string(enc, *(const char**)field_ptr);
                break;
            default:
                err = NPB_ERR_INVALID_TYPE;
        }
        
        if (err != NPB_OK) return err;
    }
    
    return NPB_OK;
}

npb_error_t npb_decode_message(npb_decoder_t* dec, void* msg, const npb_msgdesc_t* desc) {
    if (!dec || !msg || !desc) return NPB_ERR_NULL_PTR;
    
    while (npb_decoder_remaining(dec) > 0) {
        uint32_t field_id;
        uint8_t wire_type;
        
        npb_error_t err = npb_decode_tag(dec, &field_id, &wire_type);
        if (err != NPB_OK) {
            if (npb_decoder_remaining(dec) == 0) break;
            return err;
        }
        
        const npb_field_desc_t* field = find_field(desc, field_id);
        if (!field) {
            switch (wire_type) {
                case WIRETYPE_VARINT: {
                    uint64_t dummy;
                    npb_decode_varint(dec, &dummy);
                    break;
                }
                case WIRETYPE_FIXED64: {
                    uint64_t dummy;
                    npb_decode_fixed64(dec, &dummy);
                    break;
                }
                case WIRETYPE_LENGTH_DELIMITED: {
                    uint64_t len;
                    npb_decode_varint(dec, &len);
                    dec->pos += len;
                    break;
                }
                case WIRETYPE_FIXED32: {
                    uint32_t dummy;
                    npb_decode_fixed32(dec, &dummy);
                    break;
                }
                default:
                    return NPB_ERR_INVALID_TAG;
            }
            continue;
        }
        
        uint8_t* field_ptr = (uint8_t*)msg + field->offset;
        
        switch (field->type) {
            case NPB_TYPE_INT32:
            case NPB_TYPE_INT64:
            case NPB_TYPE_UINT32:
            case NPB_TYPE_UINT64:
            case NPB_TYPE_ENUM: {
                uint64_t v;
                err = npb_decode_varint(dec, &v);
                if (err == NPB_OK) *(uint64_t*)field_ptr = v;
                break;
            }
            case NPB_TYPE_SINT32: {
                uint64_t v;
                err = npb_decode_varint(dec, &v);
                if (err == NPB_OK) *(int32_t*)field_ptr = npb_zigzag_decode((uint32_t)v);
                break;
            }
            case NPB_TYPE_SINT64: {
                uint64_t v;
                err = npb_decode_varint(dec, &v);
                if (err == NPB_OK) *(int64_t*)field_ptr = npb_zigzag_decode64(v);
                break;
            }
            case NPB_TYPE_BOOL: {
                uint64_t v;
                err = npb_decode_varint(dec, &v);
                if (err == NPB_OK) *(bool*)field_ptr = (v != 0);
                break;
            }
            case NPB_TYPE_FIXED32:
            case NPB_TYPE_SFIXED32:
                err = npb_decode_fixed32(dec, (uint32_t*)field_ptr);
                break;
            case NPB_TYPE_FIXED64:
            case NPB_TYPE_SFIXED64:
                err = npb_decode_fixed64(dec, (uint64_t*)field_ptr);
                break;
            case NPB_TYPE_FLOAT:
                err = npb_decode_float(dec, (float*)field_ptr);
                break;
            case NPB_TYPE_DOUBLE:
                err = npb_decode_double(dec, (double*)field_ptr);
                break;
            case NPB_TYPE_STRING: {
                char** str_ptr = (char**)field_ptr;
                if (*str_ptr) {
                    free(*str_ptr);
                    *str_ptr = NULL;
                }
                uint64_t len;
                err = npb_decode_varint(dec, &len);
                if (err == NPB_OK) {
                    if (dec->pos + len > dec->size) {
                        err = NPB_ERR_UNDERFLOW;
                    } else {
                        *str_ptr = (char*)malloc(len + 1);
                        if (*str_ptr) {
                            memcpy(*str_ptr, dec->buffer + dec->pos, len);
                            (*str_ptr)[len] = '\0';
                            dec->pos += len;
                        } else {
                            err = NPB_ERR_OUT_OF_MEMORY;
                        }
                    }
                }
                break;
            }
            default:
                err = NPB_ERR_INVALID_TYPE;
        }
        
        if (err != NPB_OK) return err;
    }
    
    return NPB_OK;
}

int npb_get_int32(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int32_t* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_INT32:
        case NPB_TYPE_SINT32:
        case NPB_TYPE_SFIXED32:
            *value = *(const int32_t*)field_ptr;
            return 0;
        default:
            return -1;
    }
}

int npb_get_int64(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int64_t* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_INT64:
        case NPB_TYPE_SINT64:
        case NPB_TYPE_SFIXED64:
            *value = *(const int64_t*)field_ptr;
            return 0;
        default:
            return -1;
    }
}

int npb_get_uint32(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint32_t* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_UINT32:
        case NPB_TYPE_FIXED32:
            *value = *(const uint32_t*)field_ptr;
            return 0;
        default:
            return -1;
    }
}

int npb_get_uint64(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint64_t* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_UINT64:
        case NPB_TYPE_FIXED64:
            *value = *(const uint64_t*)field_ptr;
            return 0;
        default:
            return -1;
    }
}

int npb_get_bool(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, bool* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_BOOL) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    *value = *(const bool*)field_ptr;
    return 0;
}

int npb_get_float(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, float* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_FLOAT) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    *value = *(const float*)field_ptr;
    return 0;
}

int npb_get_double(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, double* value) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_DOUBLE) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    *value = *(const double*)field_ptr;
    return 0;
}

int npb_get_string(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, char* value, size_t max_len) {
    if (!msg || !desc || !value) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_STRING) return -1;
    
    const uint8_t* field_ptr = (const uint8_t*)msg + field->offset;
    const char* str = *(const char**)field_ptr;
    
    if (str) {
        strncpy(value, str, max_len - 1);
        value[max_len - 1] = '\0';
    } else {
        value[0] = '\0';
    }
    return 0;
}

int npb_set_int32(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int32_t value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_INT32:
        case NPB_TYPE_SINT32:
        case NPB_TYPE_SFIXED32:
            *(int32_t*)field_ptr = value;
            return 0;
        default:
            return -1;
    }
}

int npb_set_int64(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int64_t value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_INT64:
        case NPB_TYPE_SINT64:
        case NPB_TYPE_SFIXED64:
            *(int64_t*)field_ptr = value;
            return 0;
        default:
            return -1;
    }
}

int npb_set_uint32(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint32_t value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_UINT32:
        case NPB_TYPE_FIXED32:
            *(uint32_t*)field_ptr = value;
            return 0;
        default:
            return -1;
    }
}

int npb_set_uint64(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint64_t value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    switch (field->type) {
        case NPB_TYPE_UINT64:
        case NPB_TYPE_FIXED64:
            *(uint64_t*)field_ptr = value;
            return 0;
        default:
            return -1;
    }
}

int npb_set_bool(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, bool value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_BOOL) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    *(bool*)field_ptr = value;
    return 0;
}

int npb_set_float(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, float value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_FLOAT) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    *(float*)field_ptr = value;
    return 0;
}

int npb_set_double(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, double value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_DOUBLE) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    *(double*)field_ptr = value;
    return 0;
}

int npb_set_string(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, const char* value) {
    if (!msg || !desc) return -1;
    const npb_field_desc_t* field = find_field(desc, field_id);
    if (!field || field->type != NPB_TYPE_STRING) return -1;
    
    uint8_t* field_ptr = (uint8_t*)msg + field->offset;
    char** str_ptr = (char**)field_ptr;
    
    if (*str_ptr) free(*str_ptr);
    *str_ptr = value ? strdup(value) : NULL;
    return 0;
}

int npb_message_encode(const void* msg, const npb_msgdesc_t* desc, uint8_t* buffer, size_t* size) {
    if (!msg || !desc || !buffer || !size) return -1;
    
    npb_encoder_t* enc = npb_encoder_create_with_buffer(buffer, *size);
    if (!enc) return -1;
    
    npb_error_t err = npb_encode_message(enc, msg, desc);
    if (err != NPB_OK) {
        npb_encoder_destroy(enc);
        return -1;
    }
    
    *size = enc->pos;
    npb_encoder_destroy(enc);
    return 0;
}

int npb_message_decode(const uint8_t* buffer, size_t size, void* msg, const npb_msgdesc_t* desc) {
    if (!buffer || !msg || !desc || size == 0) return -1;
    
    npb_decoder_t* dec = npb_decoder_create(buffer, size);
    if (!dec) return -1;
    
    npb_error_t err = npb_decode_message(dec, msg, desc);
    npb_decoder_destroy(dec);
    
    return (err == NPB_OK) ? 0 : -1;
}

int npb_message_to_file(const void* msg, const npb_msgdesc_t* desc, const char* filename) {
    if (!msg || !desc || !filename) return -1;
    
    uint8_t* buffer = malloc(NPB_MAX_MESSAGE_SIZE);
    if (!buffer) return -1;
    
    size_t size = NPB_MAX_MESSAGE_SIZE;
    int result = npb_message_encode(msg, desc, buffer, &size);
    
    if (result == 0) {
        FILE* fp = fopen(filename, "wb");
        if (fp) {
            fwrite(buffer, 1, size, fp);
            fclose(fp);
        } else {
            result = -1;
        }
    }
    
    free(buffer);
    return result;
}

int npb_message_from_file(const char* filename, void* msg, const npb_msgdesc_t* desc) {
    if (!filename || !msg || !desc) return -1;
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return -1;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (size <= 0 || size > NPB_MAX_MESSAGE_SIZE) {
        fclose(fp);
        return -1;
    }
    
    uint8_t* buffer = malloc(size);
    if (!buffer) {
        fclose(fp);
        return -1;
    }
    
    fread(buffer, 1, size, fp);
    fclose(fp);
    
    int result = npb_message_decode(buffer, size, msg, desc);
    free(buffer);
    
    return result;
}
