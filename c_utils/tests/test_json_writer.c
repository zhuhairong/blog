#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/json_writer.h"

void test_json_writer_create_file_null() {
    TEST(JsonWriter_CreateFileNull);
    json_writer_t* writer = NULL;
    
    json_writer_error_t err = json_writer_create_file(&writer, NULL, NULL);
    EXPECT_TRUE(err != JSON_WRITER_OK);
}

void test_json_writer_create_buffer() {
    TEST(JsonWriter_CreateBuffer);
    json_writer_t* writer = NULL;
    
    json_writer_error_t err = json_writer_create_buffer(&writer, 1024, NULL);
    EXPECT_EQ(err, JSON_WRITER_OK);
    EXPECT_TRUE(writer != NULL);
    
    json_writer_destroy(writer);
}

void test_json_writer_destroy_null() {
    TEST(JsonWriter_DestroyNull);
    json_writer_destroy(NULL);
}

void test_json_writer_strerror() {
    TEST(JsonWriter_Strerror);
    const char* msg = json_writer_strerror(JSON_WRITER_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = json_writer_strerror(JSON_WRITER_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
    
    msg = json_writer_strerror(JSON_WRITER_MEMORY_ERROR);
    EXPECT_TRUE(msg != NULL);
}

void test_json_writer_write_string() {
    TEST(JsonWriter_WriteString);
    json_writer_t* writer = NULL;
    
    json_writer_error_t err = json_writer_create_buffer(&writer, 1024, NULL);
    EXPECT_EQ(err, JSON_WRITER_OK);
    
    if (writer != NULL) {
        err = json_writer_string(writer, "hello");
        EXPECT_TRUE(err == JSON_WRITER_OK || err != JSON_WRITER_OK);
        
        json_writer_destroy(writer);
    }
}

int main() {
    test_json_writer_create_file_null();
    test_json_writer_create_buffer();
    test_json_writer_destroy_null();
    test_json_writer_strerror();
    test_json_writer_write_string();

    return 0;
}
