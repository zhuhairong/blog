#include "zip_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>

#define LOCAL_FILE_HEADER_SIG 0x04034b50
#define CENTRAL_DIR_SIG 0x02014b50
#define END_CENTRAL_DIR_SIG 0x06054b50
#define DATA_DESCRIPTOR_SIG 0x08074b50

#define DEFAULT_BUFFER_SIZE 65536

static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdede86c5, 0x47d7927a, 0x30d0e6ec,
    0xbdc21c28, 0xcac5a8be, 0x53c2d904, 0x24c5e916, 0xbac89a3b, 0xcdbf78cd, 0x54b62931, 0x23b1569d,
    0xc30c8ea1, 0xb40bbe37, 0x2d0a3c6d, 0x5b0d1a0f, 0xc6dcb7c8, 0xb1bdb5a5, 0x28b20d8b, 0x5eb55e79
};

uint32_t zip_crc32(const void* data, size_t size) {
    if (!data || size == 0) return 0;
    
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* ptr = (const uint8_t*)data;
    
    for (size_t i = 0; i < size; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ ptr[i]) & 0xFF];
    }
    
    return crc ^ 0xFFFFFFFF;
}

const char* zip_error_string(zip_error_t error) {
    switch (error) {
        case ZIP_OK: return "Success";
        case ZIP_ERR_NULL_PTR: return "Null pointer";
        case ZIP_ERR_MEMORY: return "Memory allocation failed";
        case ZIP_ERR_FILE_NOT_FOUND: return "File not found";
        case ZIP_ERR_INVALID_ZIP: return "Invalid ZIP file";
        case ZIP_ERR_CRC_MISMATCH: return "CRC32 mismatch";
        case ZIP_ERR_COMPRESS_FAILED: return "Compression failed";
        case ZIP_ERR_DECOMPRESS_FAILED: return "Decompression failed";
        case ZIP_ERR_BUFFER_TOO_SMALL: return "Buffer too small";
        case ZIP_ERR_ENTRY_NOT_FOUND: return "Entry not found";
        case ZIP_ERR_WRITE_FAILED: return "Write failed";
        case ZIP_ERR_READ_FAILED: return "Read failed";
        case ZIP_ERR_NOT_FINISHED: return "ZIP not finished";
        case ZIP_ERR_ALREADY_FINISHED: return "ZIP already finished";
        default: return "Unknown error";
    }
}

typedef struct {
    char filename[ZIP_MAX_FILENAME];
    size_t uncompressed_size;
    size_t compressed_size;
    uint32_t crc32;
    uint16_t compression_method;
    uint32_t modified_time;
    uint32_t modified_date;
    size_t local_header_offset;
} zip_entry_t;

struct zip_writer {
    FILE* file;
    uint8_t* buffer;
    size_t buffer_capacity;
    size_t buffer_size;
    zip_entry_t* entries;
    size_t entry_count;
    size_t entry_capacity;
    bool is_memory;
    bool finished;
    zip_error_t last_error;
};

struct zip_reader {
    FILE* file;
    const uint8_t* memory_data;
    size_t memory_size;
    zip_entry_t* entries;
    size_t entry_count;
    size_t current_pos;
    bool is_memory;
    zip_error_t last_error;
};

static void get_dos_datetime(time_t t, uint16_t* dos_time, uint16_t* dos_date) {
    struct tm* tm_info = localtime(&t);
    if (tm_info) {
        *dos_time = (tm_info->tm_hour << 11) | (tm_info->tm_min << 5) | (tm_info->tm_sec / 2);
        *dos_date = ((tm_info->tm_year - 80) << 9) | ((tm_info->tm_mon + 1) << 5) | tm_info->tm_mday;
    } else {
        *dos_time = 0;
        *dos_date = 0;
    }
}

static uint16_t read_u16_le(const uint8_t* data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t read_u32_le(const uint8_t* data) {
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | 
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static void write_u16_le(uint8_t* data, uint16_t value) {
    data[0] = value & 0xFF;
    data[1] = (value >> 8) & 0xFF;
}

static void write_u32_le(uint8_t* data, uint32_t value) {
    data[0] = value & 0xFF;
    data[1] = (value >> 8) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >> 24) & 0xFF;
}

zip_writer_t* zip_writer_create(const char* filename, zip_error_t* error) {
    if (!filename) {
        if (error) *error = ZIP_ERR_NULL_PTR;
        return NULL;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        if (error) *error = ZIP_ERR_FILE_NOT_FOUND;
        return NULL;
    }
    
    zip_writer_t* writer = (zip_writer_t*)calloc(1, sizeof(zip_writer_t));
    if (!writer) {
        fclose(file);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    writer->file = file;
    writer->is_memory = false;
    writer->entry_capacity = 16;
    writer->entries = (zip_entry_t*)calloc(writer->entry_capacity, sizeof(zip_entry_t));
    
    if (!writer->entries) {
        fclose(file);
        free(writer);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    if (error) *error = ZIP_OK;
    return writer;
}

zip_writer_t* zip_writer_create_memory(zip_error_t* error) {
    zip_writer_t* writer = (zip_writer_t*)calloc(1, sizeof(zip_writer_t));
    if (!writer) {
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    writer->buffer_capacity = DEFAULT_BUFFER_SIZE;
    writer->buffer = (uint8_t*)malloc(writer->buffer_capacity);
    if (!writer->buffer) {
        free(writer);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    writer->is_memory = true;
    writer->entry_capacity = 16;
    writer->entries = (zip_entry_t*)calloc(writer->entry_capacity, sizeof(zip_entry_t));
    
    if (!writer->entries) {
        free(writer->buffer);
        free(writer);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    if (error) *error = ZIP_OK;
    return writer;
}

void zip_writer_destroy(zip_writer_t* writer) {
    if (!writer) return;
    
    if (writer->file) {
        fclose(writer->file);
    }
    if (writer->buffer) {
        free(writer->buffer);
    }
    if (writer->entries) {
        free(writer->entries);
    }
    free(writer);
}

static int ensure_buffer_capacity(zip_writer_t* writer, size_t needed) {
    if (writer->buffer_size + needed <= writer->buffer_capacity) {
        return 0;
    }
    
    size_t new_capacity = writer->buffer_capacity * 2;
    while (new_capacity < writer->buffer_size + needed) {
        new_capacity *= 2;
    }
    
    uint8_t* new_buffer = (uint8_t*)realloc(writer->buffer, new_capacity);
    if (!new_buffer) {
        return -1;
    }
    
    writer->buffer = new_buffer;
    writer->buffer_capacity = new_capacity;
    return 0;
}

static int write_data(zip_writer_t* writer, const void* data, size_t size) {
    if (writer->is_memory) {
        if (ensure_buffer_capacity(writer, size) != 0) {
            return -1;
        }
        memcpy(writer->buffer + writer->buffer_size, data, size);
        writer->buffer_size += size;
    } else {
        if (fwrite(data, 1, size, writer->file) != size) {
            return -1;
        }
    }
    return 0;
}

static size_t get_current_offset(zip_writer_t* writer) {
    if (writer->is_memory) {
        return writer->buffer_size;
    } else {
        return (size_t)ftell(writer->file);
    }
}

zip_error_t zip_writer_add_data(zip_writer_t* writer, const char* entry_name,
                                 const void* data, size_t size, zip_compress_level_t level) {
    if (!writer || !entry_name || (!data && size > 0)) {
        return ZIP_ERR_NULL_PTR;
    }
    
    if (writer->finished) {
        return ZIP_ERR_ALREADY_FINISHED;
    }
    
    if (writer->entry_count >= writer->entry_capacity) {
        size_t new_capacity = writer->entry_capacity * 2;
        zip_entry_t* new_entries = (zip_entry_t*)realloc(writer->entries, 
                                                          new_capacity * sizeof(zip_entry_t));
        if (!new_entries) {
            return ZIP_ERR_MEMORY;
        }
        writer->entries = new_entries;
        writer->entry_capacity = new_capacity;
    }
    
    zip_entry_t* entry = &writer->entries[writer->entry_count];
    memset(entry, 0, sizeof(zip_entry_t));
    
    strncpy(entry->filename, entry_name, ZIP_MAX_FILENAME - 1);
    entry->filename[ZIP_MAX_FILENAME - 1] = '\0';
    
    time_t now = time(NULL);
    uint16_t dos_time, dos_date;
    get_dos_datetime(now, &dos_time, &dos_date);
    entry->modified_time = dos_time;
    entry->modified_date = dos_date;
    
    entry->crc32 = zip_crc32(data, size);
    entry->uncompressed_size = size;
    entry->local_header_offset = get_current_offset(writer);
    
    size_t filename_len = strlen(entry->filename);
    uint8_t local_header[30];
    
    write_u32_le(local_header + 0, LOCAL_FILE_HEADER_SIG);
    write_u16_le(local_header + 4, ZIP_VERSION);
    write_u16_le(local_header + 6, 0);
    write_u16_le(local_header + 8, ZIP_METHOD_STORE);
    write_u16_le(local_header + 10, dos_time);
    write_u16_le(local_header + 12, dos_date);
    write_u32_le(local_header + 14, entry->crc32);
    write_u32_le(local_header + 18, (uint32_t)size);
    write_u32_le(local_header + 22, (uint32_t)size);
    write_u16_le(local_header + 26, (uint16_t)filename_len);
    write_u16_le(local_header + 28, 0);
    
    if (write_data(writer, local_header, 30) != 0) {
        return ZIP_ERR_WRITE_FAILED;
    }
    
    if (write_data(writer, entry->filename, filename_len) != 0) {
        return ZIP_ERR_WRITE_FAILED;
    }
    
    if (size > 0 && data) {
        if (write_data(writer, data, size) != 0) {
            return ZIP_ERR_WRITE_FAILED;
        }
    }
    
    entry->compressed_size = size;
    entry->compression_method = ZIP_METHOD_STORE;
    writer->entry_count++;
    
    return ZIP_OK;
}

zip_error_t zip_writer_add_file(zip_writer_t* writer, const char* filename, 
                                 const char* entry_name, zip_compress_level_t level) {
    if (!writer || !filename) {
        return ZIP_ERR_NULL_PTR;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return ZIP_ERR_FILE_NOT_FOUND;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(file);
        return ZIP_ERR_READ_FAILED;
    }
    
    void* data = NULL;
    size_t size = (size_t)file_size;
    
    if (size > 0) {
        data = malloc(size);
        if (!data) {
            fclose(file);
            return ZIP_ERR_MEMORY;
        }
        
        if (fread(data, 1, size, file) != size) {
            free(data);
            fclose(file);
            return ZIP_ERR_READ_FAILED;
        }
    }
    
    fclose(file);
    
    const char* name = entry_name ? entry_name : filename;
    const char* basename = strrchr(name, '/');
    if (!basename) basename = strrchr(name, '\\');
    if (basename) basename++;
    else basename = name;
    
    zip_error_t error = zip_writer_add_data(writer, basename, data, size, level);
    
    if (data) free(data);
    return error;
}

const void* zip_writer_get_buffer(zip_writer_t* writer, size_t* size) {
    if (!writer || !writer->is_memory) {
        if (size) *size = 0;
        return NULL;
    }
    
    if (size) *size = writer->buffer_size;
    return writer->buffer;
}

zip_error_t zip_writer_finish(zip_writer_t* writer) {
    if (!writer) {
        return ZIP_ERR_NULL_PTR;
    }
    
    if (writer->finished) {
        return ZIP_ERR_ALREADY_FINISHED;
    }
    
    size_t central_dir_offset = get_current_offset(writer);
    size_t central_dir_size = 0;
    
    for (size_t i = 0; i < writer->entry_count; i++) {
        zip_entry_t* entry = &writer->entries[i];
        size_t filename_len = strlen(entry->filename);
        
        uint8_t central_header[46];
        
        write_u32_le(central_header + 0, CENTRAL_DIR_SIG);
        write_u16_le(central_header + 4, ZIP_VERSION);
        write_u16_le(central_header + 6, ZIP_VERSION);
        write_u16_le(central_header + 8, 0);
        write_u16_le(central_header + 10, entry->compression_method);
        write_u16_le(central_header + 12, (uint16_t)entry->modified_time);
        write_u16_le(central_header + 14, (uint16_t)entry->modified_date);
        write_u32_le(central_header + 16, entry->crc32);
        write_u32_le(central_header + 20, (uint32_t)entry->compressed_size);
        write_u32_le(central_header + 24, (uint32_t)entry->uncompressed_size);
        write_u16_le(central_header + 28, (uint16_t)filename_len);
        write_u16_le(central_header + 30, 0);
        write_u16_le(central_header + 32, 0);
        write_u16_le(central_header + 34, 0);
        write_u16_le(central_header + 36, 0);
        write_u32_le(central_header + 38, 0);
        write_u32_le(central_header + 42, (uint32_t)entry->local_header_offset);
        
        if (write_data(writer, central_header, 46) != 0) {
            return ZIP_ERR_WRITE_FAILED;
        }
        
        if (write_data(writer, entry->filename, filename_len) != 0) {
            return ZIP_ERR_WRITE_FAILED;
        }
        
        central_dir_size += 46 + filename_len;
    }
    
    uint8_t end_record[22];
    
    write_u32_le(end_record + 0, END_CENTRAL_DIR_SIG);
    write_u16_le(end_record + 4, 0);
    write_u16_le(end_record + 6, 0);
    write_u16_le(end_record + 8, (uint16_t)writer->entry_count);
    write_u16_le(end_record + 10, (uint16_t)writer->entry_count);
    write_u32_le(end_record + 12, (uint32_t)central_dir_size);
    write_u32_le(end_record + 16, (uint32_t)central_dir_offset);
    write_u16_le(end_record + 20, 0);
    
    if (write_data(writer, end_record, 22) != 0) {
        return ZIP_ERR_WRITE_FAILED;
    }
    
    writer->finished = true;
    return ZIP_OK;
}

zip_reader_t* zip_reader_create(const char* filename, zip_error_t* error) {
    if (!filename) {
        if (error) *error = ZIP_ERR_NULL_PTR;
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        if (error) *error = ZIP_ERR_FILE_NOT_FOUND;
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, -22, SEEK_END);
    
    uint8_t end_record[22];
    if (fread(end_record, 1, 22, file) != 22) {
        fclose(file);
        if (error) *error = ZIP_ERR_READ_FAILED;
        return NULL;
    }
    
    if (read_u32_le(end_record) != END_CENTRAL_DIR_SIG) {
        fclose(file);
        if (error) *error = ZIP_ERR_INVALID_ZIP;
        return NULL;
    }
    
    uint16_t entry_count = read_u16_le(end_record + 10);
    uint32_t central_dir_size = read_u32_le(end_record + 12);
    uint32_t central_dir_offset = read_u32_le(end_record + 16);
    
    zip_reader_t* reader = (zip_reader_t*)calloc(1, sizeof(zip_reader_t));
    if (!reader) {
        fclose(file);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    reader->file = file;
    reader->is_memory = false;
    reader->entry_count = entry_count;
    reader->entries = (zip_entry_t*)calloc(entry_count, sizeof(zip_entry_t));
    
    if (!reader->entries) {
        fclose(file);
        free(reader);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    fseek(file, central_dir_offset, SEEK_SET);
    
    for (size_t i = 0; i < entry_count; i++) {
        uint8_t central_header[46];
        if (fread(central_header, 1, 46, file) != 46) {
            zip_reader_destroy(reader);
            if (error) *error = ZIP_ERR_READ_FAILED;
            return NULL;
        }
        
        if (read_u32_le(central_header) != CENTRAL_DIR_SIG) {
            zip_reader_destroy(reader);
            if (error) *error = ZIP_ERR_INVALID_ZIP;
            return NULL;
        }
        
        zip_entry_t* entry = &reader->entries[i];
        entry->compression_method = read_u16_le(central_header + 10);
        entry->modified_time = read_u16_le(central_header + 12);
        entry->modified_date = read_u16_le(central_header + 14);
        entry->crc32 = read_u32_le(central_header + 16);
        entry->compressed_size = read_u32_le(central_header + 20);
        entry->uncompressed_size = read_u32_le(central_header + 24);
        entry->local_header_offset = read_u32_le(central_header + 42);
        
        uint16_t filename_len = read_u16_le(central_header + 28);
        uint16_t extra_len = read_u16_le(central_header + 30);
        uint16_t comment_len = read_u16_le(central_header + 32);
        
        if (filename_len >= ZIP_MAX_FILENAME) {
            filename_len = ZIP_MAX_FILENAME - 1;
        }
        
        if (fread(entry->filename, 1, filename_len, file) != filename_len) {
            zip_reader_destroy(reader);
            if (error) *error = ZIP_ERR_READ_FAILED;
            return NULL;
        }
        entry->filename[filename_len] = '\0';
        
        fseek(file, extra_len + comment_len, SEEK_CUR);
    }
    
    if (error) *error = ZIP_OK;
    return reader;
}

zip_reader_t* zip_reader_create_memory(const void* data, size_t size, zip_error_t* error) {
    if (!data || size < 22) {
        if (error) *error = ZIP_ERR_NULL_PTR;
        return NULL;
    }
    
    const uint8_t* ptr = (const uint8_t*)data;
    const uint8_t* end_record = ptr + size - 22;
    
    if (read_u32_le(end_record) != END_CENTRAL_DIR_SIG) {
        if (error) *error = ZIP_ERR_INVALID_ZIP;
        return NULL;
    }
    
    uint16_t entry_count = read_u16_le(end_record + 10);
    uint32_t central_dir_size = read_u32_le(end_record + 12);
    uint32_t central_dir_offset = read_u32_le(end_record + 16);
    
    zip_reader_t* reader = (zip_reader_t*)calloc(1, sizeof(zip_reader_t));
    if (!reader) {
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    reader->memory_data = data;
    reader->memory_size = size;
    reader->is_memory = true;
    reader->entry_count = entry_count;
    reader->entries = (zip_entry_t*)calloc(entry_count, sizeof(zip_entry_t));
    
    if (!reader->entries) {
        free(reader);
        if (error) *error = ZIP_ERR_MEMORY;
        return NULL;
    }
    
    size_t pos = central_dir_offset;
    
    for (size_t i = 0; i < entry_count; i++) {
        if (pos + 46 > size) {
            zip_reader_destroy(reader);
            if (error) *error = ZIP_ERR_INVALID_ZIP;
            return NULL;
        }
        
        const uint8_t* central_header = ptr + pos;
        
        if (read_u32_le(central_header) != CENTRAL_DIR_SIG) {
            zip_reader_destroy(reader);
            if (error) *error = ZIP_ERR_INVALID_ZIP;
            return NULL;
        }
        
        zip_entry_t* entry = &reader->entries[i];
        entry->compression_method = read_u16_le(central_header + 10);
        entry->modified_time = read_u16_le(central_header + 12);
        entry->modified_date = read_u16_le(central_header + 14);
        entry->crc32 = read_u32_le(central_header + 16);
        entry->compressed_size = read_u32_le(central_header + 20);
        entry->uncompressed_size = read_u32_le(central_header + 24);
        entry->local_header_offset = read_u32_le(central_header + 42);
        
        uint16_t filename_len = read_u16_le(central_header + 28);
        uint16_t extra_len = read_u16_le(central_header + 30);
        uint16_t comment_len = read_u16_le(central_header + 32);
        
        if (filename_len >= ZIP_MAX_FILENAME) {
            filename_len = ZIP_MAX_FILENAME - 1;
        }
        
        memcpy(entry->filename, ptr + pos + 46, filename_len);
        entry->filename[filename_len] = '\0';
        
        pos += 46 + filename_len + extra_len + comment_len;
    }
    
    if (error) *error = ZIP_OK;
    return reader;
}

void zip_reader_destroy(zip_reader_t* reader) {
    if (!reader) return;
    
    if (reader->file) {
        fclose(reader->file);
    }
    if (reader->entries) {
        free(reader->entries);
    }
    free(reader);
}

size_t zip_reader_get_entry_count(zip_reader_t* reader) {
    return reader ? reader->entry_count : 0;
}

zip_error_t zip_reader_get_entry_info(zip_reader_t* reader, size_t index, zip_entry_info_t* info) {
    if (!reader || !info) {
        return ZIP_ERR_NULL_PTR;
    }
    
    if (index >= reader->entry_count) {
        return ZIP_ERR_ENTRY_NOT_FOUND;
    }
    
    zip_entry_t* entry = &reader->entries[index];
    
    strncpy(info->filename, entry->filename, ZIP_MAX_FILENAME - 1);
    info->filename[ZIP_MAX_FILENAME - 1] = '\0';
    info->uncompressed_size = entry->uncompressed_size;
    info->compressed_size = entry->compressed_size;
    info->crc32 = entry->crc32;
    info->compression_method = entry->compression_method;
    info->modified_time = entry->modified_time;
    info->modified_date = entry->modified_date;
    
    return ZIP_OK;
}

int zip_reader_find_entry(zip_reader_t* reader, const char* entry_name) {
    if (!reader || !entry_name) {
        return -1;
    }
    
    for (size_t i = 0; i < reader->entry_count; i++) {
        if (strcmp(reader->entries[i].filename, entry_name) == 0) {
            return (int)i;
        }
    }
    
    return -1;
}

static zip_error_t read_entry_data(zip_reader_t* reader, size_t index, void* buffer, size_t* size) {
    if (index >= reader->entry_count) {
        return ZIP_ERR_ENTRY_NOT_FOUND;
    }
    
    zip_entry_t* entry = &reader->entries[index];
    
    if (buffer && size && *size < entry->uncompressed_size) {
        return ZIP_ERR_BUFFER_TOO_SMALL;
    }
    
    size_t local_header_offset = entry->local_header_offset;
    
    uint8_t local_header[30];
    
    if (reader->is_memory) {
        if (local_header_offset + 30 > reader->memory_size) {
            return ZIP_ERR_INVALID_ZIP;
        }
        memcpy(local_header, reader->memory_data + local_header_offset, 30);
    } else {
        fseek(reader->file, local_header_offset, SEEK_SET);
        if (fread(local_header, 1, 30, reader->file) != 30) {
            return ZIP_ERR_READ_FAILED;
        }
    }
    
    if (read_u32_le(local_header) != LOCAL_FILE_HEADER_SIG) {
        return ZIP_ERR_INVALID_ZIP;
    }
    
    uint16_t filename_len = read_u16_le(local_header + 26);
    uint16_t extra_len = read_u16_le(local_header + 28);
    
    size_t data_offset = local_header_offset + 30 + filename_len + extra_len;
    
    if (entry->compression_method == ZIP_METHOD_STORE) {
        if (buffer) {
            if (reader->is_memory) {
                if (data_offset + entry->compressed_size > reader->memory_size) {
                    return ZIP_ERR_INVALID_ZIP;
                }
                memcpy(buffer, reader->memory_data + data_offset, entry->compressed_size);
            } else {
                fseek(reader->file, data_offset, SEEK_SET);
                if (fread(buffer, 1, entry->compressed_size, reader->file) != entry->compressed_size) {
                    return ZIP_ERR_READ_FAILED;
                }
            }
        }
        if (size) *size = entry->uncompressed_size;
    } else {
        return ZIP_ERR_DECOMPRESS_FAILED;
    }
    
    if (buffer) {
        uint32_t calculated_crc = zip_crc32(buffer, entry->uncompressed_size);
        if (calculated_crc != entry->crc32) {
            return ZIP_ERR_CRC_MISMATCH;
        }
    }
    
    return ZIP_OK;
}

zip_error_t zip_reader_extract_to_memory(zip_reader_t* reader, size_t index, 
                                          void* buffer, size_t* size) {
    if (!reader || !buffer || !size) {
        return ZIP_ERR_NULL_PTR;
    }
    
    return read_entry_data(reader, index, buffer, size);
}

static int create_parent_dirs(const char* filepath) {
    char* dir = strdup(filepath);
    if (!dir) return -1;
    
    char* last_sep = strrchr(dir, '/');
    if (last_sep) {
        *last_sep = '\0';
        char* p = dir;
        while (*p) {
            if (*p == '/') {
                *p = '\0';
                mkdir(dir, 0755);
                *p = '/';
            }
            p++;
        }
        mkdir(dir, 0755);
    }
    
    free(dir);
    return 0;
}

zip_error_t zip_reader_extract_to_file(zip_reader_t* reader, size_t index, const char* filename) {
    if (!reader || !filename) {
        return ZIP_ERR_NULL_PTR;
    }
    
    if (index >= reader->entry_count) {
        return ZIP_ERR_ENTRY_NOT_FOUND;
    }
    
    create_parent_dirs(filename);
    
    zip_entry_t* entry = &reader->entries[index];
    
    void* buffer = NULL;
    if (entry->uncompressed_size > 0) {
        buffer = malloc(entry->uncompressed_size);
        if (!buffer) {
            return ZIP_ERR_MEMORY;
        }
    }
    
    size_t size = entry->uncompressed_size;
    zip_error_t error = read_entry_data(reader, index, buffer, &size);
    
    if (error != ZIP_OK) {
        if (buffer) free(buffer);
        return error;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        if (buffer) free(buffer);
        return ZIP_ERR_WRITE_FAILED;
    }
    
    if (buffer && size > 0) {
        if (fwrite(buffer, 1, size, file) != size) {
            fclose(file);
            free(buffer);
            return ZIP_ERR_WRITE_FAILED;
        }
    }
    
    fclose(file);
    if (buffer) free(buffer);
    return ZIP_OK;
}

zip_error_t zip_reader_extract_all(zip_reader_t* reader, const char* dest_dir) {
    if (!reader || !dest_dir) {
        return ZIP_ERR_NULL_PTR;
    }
    
    for (size_t i = 0; i < reader->entry_count; i++) {
        zip_entry_info_t info;
        zip_error_t error = zip_reader_get_entry_info(reader, i, &info);
        if (error != ZIP_OK) {
            return error;
        }
        
        char output_path[512];
        snprintf(output_path, sizeof(output_path), "%s/%s", dest_dir, info.filename);
        
        error = zip_reader_extract_to_file(reader, i, output_path);
        if (error != ZIP_OK) {
            return error;
        }
    }
    
    return ZIP_OK;
}
