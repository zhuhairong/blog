#ifndef C_UTILS_NANOPB_WRAPPER_H
#define C_UTILS_NANOPB_WRAPPER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NPB_MAX_MESSAGE_SIZE 65536
#define NPB_MAX_STRING_LEN 1024
#define NPB_MAX_BYTES_LEN 4096

typedef enum {
    NPB_OK = 0,
    NPB_ERR_NULL_PTR = -1,
    NPB_ERR_BUFFER_TOO_SMALL = -2,
    NPB_ERR_INVALID_TYPE = -3,
    NPB_ERR_INVALID_FIELD = -4,
    NPB_ERR_ENCODE_FAILED = -5,
    NPB_ERR_DECODE_FAILED = -6,
    NPB_ERR_OUT_OF_MEMORY = -7,
    NPB_ERR_OVERFLOW = -8,
    NPB_ERR_UNDERFLOW = -9,
    NPB_ERR_INVALID_TAG = -10
} npb_error_t;

typedef enum {
    NPB_TYPE_INT32,
    NPB_TYPE_INT64,
    NPB_TYPE_UINT32,
    NPB_TYPE_UINT64,
    NPB_TYPE_SINT32,
    NPB_TYPE_SINT64,
    NPB_TYPE_BOOL,
    NPB_TYPE_ENUM,
    NPB_TYPE_FIXED32,
    NPB_TYPE_FIXED64,
    NPB_TYPE_SFIXED32,
    NPB_TYPE_SFIXED64,
    NPB_TYPE_FLOAT,
    NPB_TYPE_DOUBLE,
    NPB_TYPE_STRING,
    NPB_TYPE_BYTES
} npb_type_t;

typedef struct {
    uint32_t field_id;
    npb_type_t type;
    const char *name;
    size_t offset;
    size_t size;
} npb_field_desc_t;

typedef struct {
    const char *name;
    const npb_field_desc_t *fields;
    size_t field_count;
    size_t msg_size;
} npb_msgdesc_t;

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t pos;
    npb_error_t last_error;
    bool owns_buffer;
} npb_encoder_t;

typedef struct {
    const uint8_t *buffer;
    size_t size;
    size_t pos;
    npb_error_t last_error;
} npb_decoder_t;

const char* npb_error_string(npb_error_t err);

npb_encoder_t* npb_encoder_create(void);
npb_encoder_t* npb_encoder_create_with_buffer(uint8_t *buffer, size_t capacity);
void npb_encoder_destroy(npb_encoder_t* enc);
void npb_encoder_reset(npb_encoder_t* enc);
const uint8_t* npb_encoder_buffer(const npb_encoder_t* enc, size_t* size);
npb_error_t npb_encoder_last_error(const npb_encoder_t* enc);

npb_decoder_t* npb_decoder_create(const uint8_t* data, size_t size);
void npb_decoder_destroy(npb_decoder_t* dec);
void npb_decoder_reset(npb_decoder_t* dec, const uint8_t* data, size_t size);
size_t npb_decoder_remaining(const npb_decoder_t* dec);
npb_error_t npb_decoder_last_error(const npb_decoder_t* dec);

npb_error_t npb_encode_varint(npb_encoder_t* enc, uint64_t value);
npb_error_t npb_encode_signed_varint(npb_encoder_t* enc, int64_t value);
npb_error_t npb_encode_fixed32(npb_encoder_t* enc, uint32_t value);
npb_error_t npb_encode_fixed64(npb_encoder_t* enc, uint64_t value);
npb_error_t npb_encode_float(npb_encoder_t* enc, float value);
npb_error_t npb_encode_double(npb_encoder_t* enc, double value);
npb_error_t npb_encode_string(npb_encoder_t* enc, const char* str);
npb_error_t npb_encode_bytes(npb_encoder_t* enc, const uint8_t* data, size_t len);
npb_error_t npb_encode_tag(npb_encoder_t* enc, uint32_t field_id, uint8_t wire_type);

npb_error_t npb_decode_varint(npb_decoder_t* dec, uint64_t* value);
npb_error_t npb_decode_signed_varint(npb_decoder_t* dec, int64_t* value);
npb_error_t npb_decode_fixed32(npb_decoder_t* dec, uint32_t* value);
npb_error_t npb_decode_fixed64(npb_decoder_t* dec, uint64_t* value);
npb_error_t npb_decode_float(npb_decoder_t* dec, float* value);
npb_error_t npb_decode_double(npb_decoder_t* dec, double* value);
npb_error_t npb_decode_string(npb_decoder_t* dec, char* str, size_t max_len);
npb_error_t npb_decode_bytes(npb_decoder_t* dec, uint8_t* data, size_t* len, size_t max_len);
npb_error_t npb_decode_tag(npb_decoder_t* dec, uint32_t* field_id, uint8_t* wire_type);

npb_error_t npb_encode_message(npb_encoder_t* enc, const void* msg, const npb_msgdesc_t* desc);
npb_error_t npb_decode_message(npb_decoder_t* dec, void* msg, const npb_msgdesc_t* desc);

int npb_get_int32(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int32_t* value);
int npb_get_int64(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int64_t* value);
int npb_get_uint32(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint32_t* value);
int npb_get_uint64(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint64_t* value);
int npb_get_bool(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, bool* value);
int npb_get_float(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, float* value);
int npb_get_double(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, double* value);
int npb_get_string(const void* msg, const npb_msgdesc_t* desc, uint32_t field_id, char* value, size_t max_len);

int npb_set_int32(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int32_t value);
int npb_set_int64(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, int64_t value);
int npb_set_uint32(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint32_t value);
int npb_set_uint64(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, uint64_t value);
int npb_set_bool(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, bool value);
int npb_set_float(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, float value);
int npb_set_double(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, double value);
int npb_set_string(void* msg, const npb_msgdesc_t* desc, uint32_t field_id, const char* value);

int npb_message_encode(const void* msg, const npb_msgdesc_t* desc, uint8_t* buffer, size_t* size);
int npb_message_decode(const uint8_t* buffer, size_t size, void* msg, const npb_msgdesc_t* desc);

int npb_message_to_file(const void* msg, const npb_msgdesc_t* desc, const char* filename);
int npb_message_from_file(const char* filename, void* msg, const npb_msgdesc_t* desc);

size_t npb_zigzag_encode(int32_t value);
int32_t npb_zigzag_decode(uint32_t value);
size_t npb_zigzag_encode64(int64_t value);
int64_t npb_zigzag_decode64(uint64_t value);

size_t npb_varint_size(uint64_t value);

#endif
