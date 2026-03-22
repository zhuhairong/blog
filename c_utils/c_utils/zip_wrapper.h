#ifndef C_UTILS_ZIP_WRAPPER_H
#define C_UTILS_ZIP_WRAPPER_H

#include <stddef.h>
#include <stdint.h>

#define ZIP_MAX_FILENAME 256
#define ZIP_MAX_COMMENT 256
#define ZIP_VERSION 20

typedef enum {
    ZIP_OK = 0,
    ZIP_ERR_NULL_PTR = -1,
    ZIP_ERR_MEMORY = -2,
    ZIP_ERR_FILE_NOT_FOUND = -3,
    ZIP_ERR_INVALID_ZIP = -4,
    ZIP_ERR_CRC_MISMATCH = -5,
    ZIP_ERR_COMPRESS_FAILED = -6,
    ZIP_ERR_DECOMPRESS_FAILED = -7,
    ZIP_ERR_BUFFER_TOO_SMALL = -8,
    ZIP_ERR_ENTRY_NOT_FOUND = -9,
    ZIP_ERR_WRITE_FAILED = -10,
    ZIP_ERR_READ_FAILED = -11,
    ZIP_ERR_NOT_FINISHED = -12,
    ZIP_ERR_ALREADY_FINISHED = -13,
} zip_error_t;

typedef enum {
    ZIP_COMPRESS_NONE = 0,
    ZIP_COMPRESS_FAST = 1,
    ZIP_COMPRESS_DEFAULT = 6,
    ZIP_COMPRESS_BEST = 9,
} zip_compress_level_t;

typedef enum {
    ZIP_METHOD_STORE = 0,
    ZIP_METHOD_DEFLATE = 8,
} zip_compression_method_t;

typedef struct {
    char filename[ZIP_MAX_FILENAME];
    size_t uncompressed_size;
    size_t compressed_size;
    uint32_t crc32;
    uint16_t compression_method;
    uint32_t modified_time;
    uint32_t modified_date;
} zip_entry_info_t;

typedef struct zip_writer zip_writer_t;
typedef struct zip_reader zip_reader_t;

const char* zip_error_string(zip_error_t error);

zip_writer_t* zip_writer_create(const char* filename, zip_error_t* error);
zip_writer_t* zip_writer_create_memory(zip_error_t* error);
void zip_writer_destroy(zip_writer_t* writer);
zip_error_t zip_writer_add_file(zip_writer_t* writer, const char* filename, 
                                 const char* entry_name, zip_compress_level_t level);
zip_error_t zip_writer_add_data(zip_writer_t* writer, const char* entry_name,
                                 const void* data, size_t size, zip_compress_level_t level);
const void* zip_writer_get_buffer(zip_writer_t* writer, size_t* size);
zip_error_t zip_writer_finish(zip_writer_t* writer);

zip_reader_t* zip_reader_create(const char* filename, zip_error_t* error);
zip_reader_t* zip_reader_create_memory(const void* data, size_t size, zip_error_t* error);
void zip_reader_destroy(zip_reader_t* reader);
size_t zip_reader_get_entry_count(zip_reader_t* reader);
zip_error_t zip_reader_get_entry_info(zip_reader_t* reader, size_t index, zip_entry_info_t* info);
int zip_reader_find_entry(zip_reader_t* reader, const char* entry_name);
zip_error_t zip_reader_extract_to_file(zip_reader_t* reader, size_t index, const char* filename);
zip_error_t zip_reader_extract_to_memory(zip_reader_t* reader, size_t index, 
                                          void* buffer, size_t* size);
zip_error_t zip_reader_extract_all(zip_reader_t* reader, const char* dest_dir);

uint32_t zip_crc32(const void* data, size_t size);

#endif
