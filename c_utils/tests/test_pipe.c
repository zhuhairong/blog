#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/pipe.h"

void test_pipe_default_config() {
    TEST(Pipe_DefaultConfig);
    pipe_config_t config = pipe_default_config();
    EXPECT_EQ(config.type, PIPE_TYPE_ANONYMOUS);
    EXPECT_TRUE(config.name == NULL);
    EXPECT_FALSE(config.non_blocking);
    EXPECT_EQ(config.buffer_size, (size_t)4096);
    EXPECT_EQ(config.timeout_ms, 0);
}

void test_pipe_create_and_close() {
    TEST(Pipe_CreateAndClose);
    pipe_t p;
    bool success = pipe_create(&p);
    EXPECT_TRUE(success);
    EXPECT_TRUE(p.is_open);
    pipe_close(&p);
}

void test_pipe_create_null() {
    TEST(Pipe_CreateNull);
    bool success = pipe_create(NULL);
    EXPECT_FALSE(success);
}

void test_pipe_create_ex() {
    TEST(Pipe_CreateEx);
    pipe_t p;
    pipe_config_t config = pipe_default_config();
    pipe_error_t error;
    bool success = pipe_create_ex(&p, &config, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_TRUE(p.is_open);
    pipe_close(&p);
}

void test_pipe_create_ex_null() {
    TEST(Pipe_CreateExNull);
    pipe_error_t error;
    bool success = pipe_create_ex(NULL, NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
}

void test_pipe_write_and_read() {
    TEST(Pipe_WriteAndRead);
    pipe_t p;
    pipe_create(&p);
    
    const char* test_data = "Hello Pipe!";
    size_t bytes_written;
    pipe_error_t error;
    bool success = pipe_write(&p, test_data, strlen(test_data), &bytes_written, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_EQ(bytes_written, strlen(test_data));
    
    char read_buf[64];
    size_t bytes_read;
    success = pipe_read(&p, read_buf, sizeof(read_buf), &bytes_read, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_EQ(bytes_read, strlen(test_data));
    read_buf[bytes_read] = '\0';
    EXPECT_STREQ(read_buf, test_data);
    
    pipe_close(&p);
}

void test_pipe_read_null() {
    TEST(Pipe_ReadNull);
    pipe_t p;
    pipe_create(&p);
    size_t bytes_read;
    pipe_error_t error;
    bool success = pipe_read(&p, NULL, 64, &bytes_read, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
    pipe_close(&p);
}

void test_pipe_write_null() {
    TEST(Pipe_WriteNull);
    pipe_t p;
    pipe_create(&p);
    size_t bytes_written;
    pipe_error_t error;
    bool success = pipe_write(&p, NULL, 64, &bytes_written, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
    pipe_close(&p);
}

void test_pipe_write_line_and_read_line() {
    TEST(Pipe_WriteLineAndReadLine);
    pipe_t p;
    pipe_create(&p);
    
    const char* test_line = "Hello Pipe Line!";
    pipe_error_t error;
    bool success = pipe_write_line(&p, test_line, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    
    char read_buf[64];
    size_t bytes_read;
    success = pipe_read_line(&p, read_buf, sizeof(read_buf), &bytes_read, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_TRUE(strlen(read_buf) > 0);
    EXPECT_TRUE(strstr(read_buf, test_line) != NULL);
    
    pipe_close(&p);
}

void test_pipe_is_readable_and_writable() {
    TEST(Pipe_IsReadableAndWritable);
    pipe_t p;
    pipe_create(&p);
    
    pipe_error_t error;
    bool writable = pipe_is_writable(&p, 100, &error);
    EXPECT_TRUE(writable);
    EXPECT_EQ(error, PIPE_OK);
    
    const char* test_data = "Test";
    pipe_write(&p, test_data, strlen(test_data), NULL, NULL);
    
    bool readable = pipe_is_readable(&p, 100, &error);
    EXPECT_TRUE(readable);
    EXPECT_EQ(error, PIPE_OK);
    
    pipe_close(&p);
}

void test_pipe_set_non_blocking() {
    TEST(Pipe_SetNonBlocking);
    pipe_t p;
    pipe_create(&p);
    
    pipe_error_t error;
    bool success = pipe_set_non_blocking(&p, true, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_TRUE(p.config.non_blocking);
    
    success = pipe_set_non_blocking(&p, false, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_FALSE(p.config.non_blocking);
    
    pipe_close(&p);
}

void test_pipe_set_non_blocking_null() {
    TEST(Pipe_SetNonBlockingNull);
    pipe_error_t error;
    bool success = pipe_set_non_blocking(NULL, true, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
}

void test_pipe_get_state() {
    TEST(Pipe_GetState);
    pipe_t p;
    pipe_create(&p);
    
    bool is_open;
    pipe_error_t error;
    bool success = pipe_get_state(&p, &is_open, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_TRUE(is_open);
    
    pipe_close(&p);
    success = pipe_get_state(&p, &is_open, &error);
    EXPECT_TRUE(success);
    EXPECT_FALSE(is_open);
}

void test_pipe_get_state_null() {
    TEST(Pipe_GetStateNull);
    pipe_error_t error;
    bool success = pipe_get_state(NULL, NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
}

void test_pipe_close_ex() {
    TEST(Pipe_CloseEx);
    pipe_t p;
    pipe_create(&p);
    
    pipe_error_t error;
    bool success = pipe_close_ex(&p, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PIPE_OK);
    EXPECT_FALSE(p.is_open);
}

void test_pipe_close_ex_null() {
    TEST(Pipe_CloseExNull);
    pipe_error_t error;
    bool success = pipe_close_ex(NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PIPE_ERROR_NULL_PTR);
}

void test_pipe_error_string() {
    TEST(Pipe_ErrorString);
    const char* msg = pipe_error_string(PIPE_OK);
    EXPECT_STREQ(msg, "Success");
    msg = pipe_error_string(PIPE_ERROR_NULL_PTR);
    EXPECT_STREQ(msg, "Null pointer error");
    msg = pipe_error_string(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_pipe_default_config();
    test_pipe_create_and_close();
    test_pipe_create_null();
    test_pipe_create_ex();
    test_pipe_create_ex_null();
    test_pipe_write_and_read();
    test_pipe_read_null();
    test_pipe_write_null();
    test_pipe_write_line_and_read_line();
    test_pipe_is_readable_and_writable();
    test_pipe_set_non_blocking();
    test_pipe_set_non_blocking_null();
    test_pipe_get_state();
    test_pipe_get_state_null();
    test_pipe_close_ex();
    test_pipe_close_ex_null();
    test_pipe_error_string();

    return 0;
}
