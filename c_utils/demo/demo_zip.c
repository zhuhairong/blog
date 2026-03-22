#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "zip_wrapper.h"

static void demo_create_zip_file(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    创建ZIP文件演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create("/tmp/demo.zip", &error);
    if (!writer) {
        printf("创建ZIP写入器失败: %s\n", zip_error_string(error));
        return;
    }
    
    printf("添加文件到ZIP...\n");
    
    error = zip_writer_add_data(writer, "hello.txt", "Hello, World!", 13, ZIP_COMPRESS_NONE);
    if (error != ZIP_OK) {
        printf("添加hello.txt失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    printf("  添加 hello.txt: \"Hello, World!\" ✓\n");
    
    error = zip_writer_add_data(writer, "data/test.txt", "This is a test file.", 20, ZIP_COMPRESS_NONE);
    if (error != ZIP_OK) {
        printf("添加test.txt失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    printf("  添加 data/test.txt: \"This is a test file.\" ✓\n");
    
    error = zip_writer_add_data(writer, "numbers.txt", "0123456789", 10, ZIP_COMPRESS_NONE);
    if (error != ZIP_OK) {
        printf("添加numbers.txt失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    printf("  添加 numbers.txt: \"0123456789\" ✓\n");
    
    error = zip_writer_finish(writer);
    if (error != ZIP_OK) {
        printf("完成ZIP失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    printf("\nZIP文件创建成功: /tmp/demo.zip ✓\n");
    
    zip_writer_destroy(writer);
}

static void demo_memory_zip(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    内存ZIP操作演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    if (!writer) {
        printf("创建内存ZIP写入器失败: %s\n", zip_error_string(error));
        return;
    }
    
    printf("在内存中创建ZIP...\n");
    
    const char* json_data = "{\"name\": \"test\", \"value\": 123}";
    error = zip_writer_add_data(writer, "config.json", json_data, strlen(json_data), ZIP_COMPRESS_NONE);
    if (error != ZIP_OK) {
        printf("添加config.json失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    printf("  添加 config.json: '%s' ✓\n", json_data);
    
    error = zip_writer_finish(writer);
    if (error != ZIP_OK) {
        printf("完成ZIP失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    
    size_t zip_size;
    const void* zip_data = zip_writer_get_buffer(writer, &zip_size);
    printf("\n内存ZIP创建成功，大小: %zu 字节 ✓\n", zip_size);
    
    printf("\n从内存ZIP读取...\n");
    zip_reader_t* reader = zip_reader_create_memory(zip_data, zip_size, &error);
    if (!reader) {
        printf("创建内存ZIP读取器失败: %s\n", zip_error_string(error));
        zip_writer_destroy(writer);
        return;
    }
    
    printf("  条目数量: %zu\n", zip_reader_get_entry_count(reader));
    
    zip_entry_info_t info;
    error = zip_reader_get_entry_info(reader, 0, &info);
    if (error == ZIP_OK) {
        printf("  文件名: %s\n", info.filename);
        printf("  原始大小: %zu 字节\n", info.uncompressed_size);
        printf("  压缩大小: %zu 字节\n", info.compressed_size);
        printf("  CRC32: 0x%08X\n", info.crc32);
    }
    
    char buffer[256];
    size_t buf_size = sizeof(buffer);
    error = zip_reader_extract_to_memory(reader, 0, buffer, &buf_size);
    if (error == ZIP_OK) {
        buffer[buf_size] = '\0';
        printf("  解压内容: '%s' ✓\n", buffer);
    }
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

static void demo_read_zip(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    读取ZIP文件演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    zip_error_t error;
    zip_reader_t* reader = zip_reader_create("/tmp/demo.zip", &error);
    if (!reader) {
        printf("打开ZIP文件失败: %s\n", zip_error_string(error));
        return;
    }
    
    size_t count = zip_reader_get_entry_count(reader);
    printf("ZIP文件内容列表:\n");
    printf("───────────────────────────────────────────────────────────────\n");
    printf("  %-20s %12s %12s %10s\n", "文件名", "原始大小", "压缩大小", "CRC32");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for (size_t i = 0; i < count; i++) {
        zip_entry_info_t info;
        error = zip_reader_get_entry_info(reader, i, &info);
        if (error == ZIP_OK) {
            printf("  %-20s %12zu %12zu 0x%08X\n", 
                   info.filename, info.uncompressed_size, info.compressed_size, info.crc32);
        }
    }
    
    printf("\n解压缩文件...\n");
    
    char buffer[256];
    size_t buf_size;
    
    int index = zip_reader_find_entry(reader, "hello.txt");
    if (index >= 0) {
        buf_size = sizeof(buffer);
        error = zip_reader_extract_to_memory(reader, index, buffer, &buf_size);
        if (error == ZIP_OK) {
            buffer[buf_size] = '\0';
            printf("  解压 hello.txt: \"%s\" ✓\n", buffer);
        }
    }
    
    index = zip_reader_find_entry(reader, "numbers.txt");
    if (index >= 0) {
        buf_size = sizeof(buffer);
        error = zip_reader_extract_to_memory(reader, index, buffer, &buf_size);
        if (error == ZIP_OK) {
            buffer[buf_size] = '\0';
            printf("  解压 numbers.txt: \"%s\" ✓\n", buffer);
        }
    }
    
    zip_reader_destroy(reader);
}

static void demo_crc32(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    CRC32计算演示                               \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    const char* test_data[] = {
        "",
        "a",
        "hello",
        "Hello, World!",
        "123456789",
    };
    
    printf("数据                CRC32值\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for (size_t i = 0; i < sizeof(test_data) / sizeof(test_data[0]); i++) {
        uint32_t crc = zip_crc32(test_data[i], strlen(test_data[i]));
        printf("\"%-16s\" 0x%08X\n", test_data[i], crc);
    }
}

static void demo_extract_all(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    解压缩所有文件演示                          \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    zip_error_t error;
    zip_reader_t* reader = zip_reader_create("/tmp/demo.zip", &error);
    if (!reader) {
        printf("打开ZIP文件失败: %s\n", zip_error_string(error));
        return;
    }
    
    mkdir("/tmp/zip_extract", 0755);
    
    error = zip_reader_extract_all(reader, "/tmp/zip_extract");
    if (error == ZIP_OK) {
        printf("所有文件解压到 /tmp/zip_extract/ ✓\n");
    } else {
        printf("解压失败: %s\n", zip_error_string(error));
    }
    
    zip_reader_destroy(reader);
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    ZIP封装库演示程序                           \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    demo_create_zip_file();
    demo_memory_zip();
    demo_read_zip();
    demo_crc32();
    demo_extract_all();
    
    printf("\n演示完成！\n\n");
    return 0;
}
