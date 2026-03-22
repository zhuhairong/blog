#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zip_wrapper.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("  测试: %s... ", #name); \
    test_##name(); \
    printf("通过\n"); \
    tests_passed++; \
} while(0)

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("失败: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("失败: %s:%d: %s != %s\n", __FILE__, __LINE__, #a, #b); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ_STR(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        printf("失败: %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); \
        tests_failed++; \
        return; \
    } \
} while(0)

TEST(error_string) {
    const char* str = zip_error_string(ZIP_OK);
    ASSERT_TRUE(str != NULL);
    ASSERT_TRUE(strlen(str) > 0);
    
    str = zip_error_string(ZIP_ERR_NULL_PTR);
    ASSERT_TRUE(str != NULL);
    
    str = zip_error_string(ZIP_ERR_MEMORY);
    ASSERT_TRUE(str != NULL);
    
    str = zip_error_string((zip_error_t)-999);
    ASSERT_TRUE(str != NULL);
}

TEST(crc32_basic) {
    uint32_t crc = zip_crc32("", 0);
    ASSERT_EQ(crc, 0);
    
    crc = zip_crc32("a", 1);
    ASSERT_EQ(crc, 0xE8B7BE43);
    
    crc = zip_crc32("123456789", 9);
    ASSERT_EQ(crc, 0xCBF43926);
    
    crc = zip_crc32("hello", 5);
    ASSERT_TRUE(crc != 0);
}

TEST(crc32_null) {
    uint32_t crc = zip_crc32(NULL, 0);
    ASSERT_EQ(crc, 0);
    
    crc = zip_crc32(NULL, 10);
    ASSERT_EQ(crc, 0);
}

TEST(writer_create_destroy) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create("/tmp/test_create.zip", &error);
    ASSERT_TRUE(writer != NULL);
    ASSERT_EQ(error, ZIP_OK);
    
    zip_writer_destroy(writer);
}

TEST(writer_create_memory) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    ASSERT_EQ(error, ZIP_OK);
    
    zip_writer_destroy(writer);
}

TEST(writer_null_handling) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create(NULL, &error);
    ASSERT_TRUE(writer == NULL);
    ASSERT_EQ(error, ZIP_ERR_NULL_PTR);
    
    writer = zip_writer_create_memory(NULL);
    ASSERT_TRUE(writer != NULL);
    zip_writer_destroy(writer);
}

TEST(writer_add_data) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "Hello", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_add_data(writer, "empty.txt", "", 0, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    zip_writer_destroy(writer);
}

TEST(writer_add_data_null) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(NULL, "test.txt", "data", 4, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_ERR_NULL_PTR);
    
    error = zip_writer_add_data(writer, NULL, "data", 4, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_ERR_NULL_PTR);
    
    zip_writer_destroy(writer);
}

TEST(writer_finish) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "Hello, World!", 13, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    ASSERT_TRUE(buffer != NULL);
    ASSERT_TRUE(size > 0);
    
    zip_writer_destroy(writer);
}

TEST(writer_double_finish) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "data", 4, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_ERR_ALREADY_FINISHED);
    
    zip_writer_destroy(writer);
}

TEST(writer_add_after_finish) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "data", 4, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_add_data(writer, "test2.txt", "data2", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_ERR_ALREADY_FINISHED);
    
    zip_writer_destroy(writer);
}

TEST(reader_memory_basic) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "Hello", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    ASSERT_TRUE(buffer != NULL);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    ASSERT_EQ(error, ZIP_OK);
    
    ASSERT_EQ(zip_reader_get_entry_count(reader), 1);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_get_entry_info) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    const char* content = "Hello, World!";
    error = zip_writer_add_data(writer, "hello.txt", content, strlen(content), ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    zip_entry_info_t info;
    error = zip_reader_get_entry_info(reader, 0, &info);
    ASSERT_EQ(error, ZIP_OK);
    ASSERT_EQ_STR(info.filename, "hello.txt");
    ASSERT_EQ(info.uncompressed_size, strlen(content));
    ASSERT_EQ(info.compressed_size, strlen(content));
    ASSERT_EQ(info.compression_method, ZIP_METHOD_STORE);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_find_entry) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "file1.txt", "data1", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_add_data(writer, "file2.txt", "data2", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_add_data(writer, "file3.txt", "data3", 5, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    int index = zip_reader_find_entry(reader, "file2.txt");
    ASSERT_EQ(index, 1);
    
    index = zip_reader_find_entry(reader, "file1.txt");
    ASSERT_EQ(index, 0);
    
    index = zip_reader_find_entry(reader, "nonexistent.txt");
    ASSERT_EQ(index, -1);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_extract_to_memory) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    const char* content = "Test content for extraction";
    error = zip_writer_add_data(writer, "extract.txt", content, strlen(content), ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    char output[256];
    size_t output_size = sizeof(output);
    error = zip_reader_extract_to_memory(reader, 0, output, &output_size);
    ASSERT_EQ(error, ZIP_OK);
    ASSERT_EQ(output_size, strlen(content));
    output[output_size] = '\0';
    ASSERT_EQ_STR(output, content);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_extract_buffer_too_small) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    const char* content = "This is a longer content string";
    error = zip_writer_add_data(writer, "test.txt", content, strlen(content), ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    char output[5];
    size_t output_size = sizeof(output);
    error = zip_reader_extract_to_memory(reader, 0, output, &output_size);
    ASSERT_EQ(error, ZIP_ERR_BUFFER_TOO_SMALL);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_extract_to_file) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    const char* content = "File content for extraction test";
    error = zip_writer_add_data(writer, "output.txt", content, strlen(content), ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    error = zip_reader_extract_to_file(reader, 0, "/tmp/extracted_test.txt");
    ASSERT_EQ(error, ZIP_OK);
    
    FILE* file = fopen("/tmp/extracted_test.txt", "r");
    ASSERT_TRUE(file != NULL);
    
    char read_content[256];
    size_t read_size = fread(read_content, 1, sizeof(read_content) - 1, file);
    fclose(file);
    read_content[read_size] = '\0';
    
    ASSERT_EQ_STR(read_content, content);
    
    unlink("/tmp/extracted_test.txt");
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_invalid_index) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "data", 4, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    
    zip_entry_info_t info;
    error = zip_reader_get_entry_info(reader, 100, &info);
    ASSERT_EQ(error, ZIP_ERR_ENTRY_NOT_FOUND);
    
    char output[256];
    size_t output_size = sizeof(output);
    error = zip_reader_extract_to_memory(reader, 100, output, &output_size);
    ASSERT_EQ(error, ZIP_ERR_ENTRY_NOT_FOUND);
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(reader_null_handling) {
    zip_error_t error;
    
    zip_reader_t* reader = zip_reader_create(NULL, &error);
    ASSERT_TRUE(reader == NULL);
    ASSERT_EQ(error, ZIP_ERR_NULL_PTR);
    
    reader = zip_reader_create_memory(NULL, 100, &error);
    ASSERT_TRUE(reader == NULL);
    ASSERT_EQ(error, ZIP_ERR_NULL_PTR);
    
    reader = zip_reader_create_memory("invalid", 7, &error);
    ASSERT_TRUE(reader == NULL);
}

TEST(multiple_entries) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create_memory(&error);
    ASSERT_TRUE(writer != NULL);
    
    for (int i = 0; i < 10; i++) {
        char filename[32];
        char content[32];
        snprintf(filename, sizeof(filename), "file%d.txt", i);
        snprintf(content, sizeof(content), "content%d", i);
        error = zip_writer_add_data(writer, filename, content, strlen(content), ZIP_COMPRESS_NONE);
        ASSERT_EQ(error, ZIP_OK);
    }
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    size_t size;
    const void* buffer = zip_writer_get_buffer(writer, &size);
    
    zip_reader_t* reader = zip_reader_create_memory(buffer, size, &error);
    ASSERT_TRUE(reader != NULL);
    ASSERT_EQ(zip_reader_get_entry_count(reader), 10);
    
    for (int i = 0; i < 10; i++) {
        char filename[32];
        char expected_content[32];
        snprintf(filename, sizeof(filename), "file%d.txt", i);
        snprintf(expected_content, sizeof(expected_content), "content%d", i);
        
        int index = zip_reader_find_entry(reader, filename);
        ASSERT_TRUE(index >= 0);
        
        char output[32];
        size_t output_size = sizeof(output);
        error = zip_reader_extract_to_memory(reader, index, output, &output_size);
        ASSERT_EQ(error, ZIP_OK);
        output[output_size] = '\0';
        ASSERT_EQ_STR(output, expected_content);
    }
    
    zip_reader_destroy(reader);
    zip_writer_destroy(writer);
}

TEST(writer_file_based) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create("/tmp/file_test.zip", &error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "test.txt", "File test content", 17, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    
    zip_writer_destroy(writer);
    
    zip_reader_t* reader = zip_reader_create("/tmp/file_test.zip", &error);
    ASSERT_TRUE(reader != NULL);
    ASSERT_EQ(zip_reader_get_entry_count(reader), 1);
    
    zip_entry_info_t info;
    error = zip_reader_get_entry_info(reader, 0, &info);
    ASSERT_EQ(error, ZIP_OK);
    ASSERT_EQ_STR(info.filename, "test.txt");
    
    zip_reader_destroy(reader);
    unlink("/tmp/file_test.zip");
}

TEST(extract_all) {
    zip_error_t error;
    zip_writer_t* writer = zip_writer_create("/tmp/extract_all_test.zip", &error);
    ASSERT_TRUE(writer != NULL);
    
    error = zip_writer_add_data(writer, "file1.txt", "content1", 8, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_add_data(writer, "file2.txt", "content2", 8, ZIP_COMPRESS_NONE);
    ASSERT_EQ(error, ZIP_OK);
    
    error = zip_writer_finish(writer);
    ASSERT_EQ(error, ZIP_OK);
    zip_writer_destroy(writer);
    
    mkdir("/tmp/extract_all_dir", 0755);
    
    zip_reader_t* reader = zip_reader_create("/tmp/extract_all_test.zip", &error);
    ASSERT_TRUE(reader != NULL);
    
    error = zip_reader_extract_all(reader, "/tmp/extract_all_dir");
    ASSERT_EQ(error, ZIP_OK);
    
    zip_reader_destroy(reader);
    
    FILE* f1 = fopen("/tmp/extract_all_dir/file1.txt", "r");
    ASSERT_TRUE(f1 != NULL);
    fclose(f1);
    
    FILE* f2 = fopen("/tmp/extract_all_dir/file2.txt", "r");
    ASSERT_TRUE(f2 != NULL);
    fclose(f2);
    
    unlink("/tmp/extract_all_test.zip");
    unlink("/tmp/extract_all_dir/file1.txt");
    unlink("/tmp/extract_all_dir/file2.txt");
    rmdir("/tmp/extract_all_dir");
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    ZIP封装库测试用例                           \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("错误处理测试:\n");
    RUN_TEST(error_string);
    
    printf("\nCRC32测试:\n");
    RUN_TEST(crc32_basic);
    RUN_TEST(crc32_null);
    
    printf("\n写入器测试:\n");
    RUN_TEST(writer_create_destroy);
    RUN_TEST(writer_create_memory);
    RUN_TEST(writer_null_handling);
    RUN_TEST(writer_add_data);
    RUN_TEST(writer_add_data_null);
    RUN_TEST(writer_finish);
    RUN_TEST(writer_double_finish);
    RUN_TEST(writer_add_after_finish);
    
    printf("\n读取器测试:\n");
    RUN_TEST(reader_memory_basic);
    RUN_TEST(reader_get_entry_info);
    RUN_TEST(reader_find_entry);
    RUN_TEST(reader_extract_to_memory);
    RUN_TEST(reader_extract_buffer_too_small);
    RUN_TEST(reader_extract_to_file);
    RUN_TEST(reader_invalid_index);
    RUN_TEST(reader_null_handling);
    
    printf("\n多条目测试:\n");
    RUN_TEST(multiple_entries);
    
    printf("\n文件操作测试:\n");
    RUN_TEST(writer_file_based);
    RUN_TEST(extract_all);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("测试结果: 通过 %d, 失败 %d\n", tests_passed, tests_failed);
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return tests_failed > 0 ? 1 : 0;
}
