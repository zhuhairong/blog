#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/csv_writer.h"

void test_csv_writer_default_config() {
    TEST(CsvWriter_DefaultConfig);
    csv_writer_config_t config = csv_writer_default_config();
    EXPECT_EQ(config.delimiter, ',');
    EXPECT_EQ(config.quote, '"');
    EXPECT_EQ(config.escape, '"');
    EXPECT_FALSE(config.quote_all_fields);
    EXPECT_FALSE(config.trim_whitespace);
    EXPECT_EQ(config.buffer_size, (size_t)4096);
}

void test_csv_writer_create_and_free() {
    TEST(CsvWriter_CreateAndFree);
    const char* test_file = "test_csv_writer.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    EXPECT_EQ(error, CSV_WRITER_OK);
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_create_null() {
    TEST(CsvWriter_CreateNull);
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(NULL, "w", NULL, &error);
    EXPECT_TRUE(writer == NULL);
    EXPECT_EQ(error, CSV_WRITER_ERROR_INVALID_PARAM);
}

void test_csv_writer_create_from_file() {
    TEST(CsvWriter_CreateFromFile);
    const char* test_file = "test_csv_from_file.csv";
    FILE* f = fopen(test_file, "w");
    EXPECT_TRUE(f != NULL);
    
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create_from_file(f, NULL, &error);
    EXPECT_TRUE(writer != NULL);
    EXPECT_EQ(error, CSV_WRITER_OK);
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_create_from_file_null() {
    TEST(CsvWriter_CreateFromFileNull);
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create_from_file(NULL, NULL, &error);
    EXPECT_TRUE(writer == NULL);
    EXPECT_EQ(error, CSV_WRITER_ERROR_INVALID_PARAM);
}

void test_csv_writer_write_field() {
    TEST(CsvWriter_WriteField);
    const char* test_file = "test_csv_write_field.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    bool success = csv_writer_write_field(writer, "Hello", false);
    EXPECT_TRUE(success);
    success = csv_writer_write_field(writer, "World", true);
    EXPECT_TRUE(success);
    
    csv_writer_free(writer);
    
    FILE* f = fopen(test_file, "r");
    char buf[256];
    fgets(buf, sizeof(buf), f);
    fclose(f);
    EXPECT_TRUE(strstr(buf, "Hello") != NULL);
    EXPECT_TRUE(strstr(buf, "World") != NULL);
    
    remove(test_file);
}

void test_csv_writer_write_field_null() {
    TEST(CsvWriter_WriteFieldNull);
    const char* test_file = "test_csv_write_field_null.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    bool success = csv_writer_write_field(writer, NULL, false);
    EXPECT_FALSE(success);
    EXPECT_TRUE(csv_writer_has_error(writer, NULL, NULL));
    
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_write_row() {
    TEST(CsvWriter_WriteRow);
    const char* test_file = "test_csv_write_row.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    const char* row[] = {"Name", "Age", "City"};
    bool success = csv_writer_write_row(writer, row, 3);
    EXPECT_TRUE(success);
    
    const char* row2[] = {"John", "30", "New York"};
    success = csv_writer_write_row(writer, row2, 3);
    EXPECT_TRUE(success);
    
    csv_writer_free(writer);
    
    FILE* f = fopen(test_file, "r");
    char buf[256];
    int lines = 0;
    while (fgets(buf, sizeof(buf), f)) {
        lines++;
    }
    fclose(f);
    EXPECT_EQ(lines, 2);
    
    remove(test_file);
}

void test_csv_writer_write_row_null() {
    TEST(CsvWriter_WriteRowNull);
    const char* test_file = "test_csv_write_row_null.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    bool success = csv_writer_write_row(writer, NULL, 3);
    EXPECT_FALSE(success);
    EXPECT_TRUE(csv_writer_has_error(writer, NULL, NULL));
    
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_write_row_va() {
    TEST(CsvWriter_WriteRowVa);
    const char* test_file = "test_csv_write_row_va.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    bool success = csv_writer_write_row_va(writer, "Product", "Price", "Quantity", NULL);
    EXPECT_TRUE(success);
    
    success = csv_writer_write_row_va(writer, "Apple", "1.99", "10", NULL);
    EXPECT_TRUE(success);
    
    csv_writer_free(writer);
    
    FILE* f = fopen(test_file, "r");
    char buf[256];
    int lines = 0;
    while (fgets(buf, sizeof(buf), f)) {
        lines++;
    }
    fclose(f);
    EXPECT_EQ(lines, 2);
    
    remove(test_file);
}

void test_csv_writer_flush() {
    TEST(CsvWriter_Flush);
    const char* test_file = "test_csv_flush.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    csv_writer_write_field(writer, "Test", true);
    bool success = csv_writer_flush(writer);
    EXPECT_TRUE(success);
    
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_has_error() {
    TEST(CsvWriter_HasError);
    const char* test_file = "test_csv_has_error.csv";
    csv_writer_error_t error;
    csv_writer_t* writer = csv_writer_create(test_file, "w", NULL, &error);
    EXPECT_TRUE(writer != NULL);
    
    EXPECT_FALSE(csv_writer_has_error(writer, NULL, NULL));
    
    csv_writer_write_field(writer, NULL, false);
    
    csv_writer_error_t error_code;
    const char* error_msg;
    EXPECT_TRUE(csv_writer_has_error(writer, &error_code, &error_msg));
    EXPECT_EQ(error_code, CSV_WRITER_ERROR_INVALID_PARAM);
    
    csv_writer_free(writer);
    remove(test_file);
}

void test_csv_writer_strerror() {
    TEST(CsvWriter_Strerror);
    const char* msg = csv_writer_strerror(CSV_WRITER_OK);
    EXPECT_STREQ(msg, "Success");
    msg = csv_writer_strerror(CSV_WRITER_ERROR_FILE_OPEN);
    EXPECT_STREQ(msg, "Failed to open file");
    msg = csv_writer_strerror(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_csv_writer_default_config();
    test_csv_writer_create_and_free();
    test_csv_writer_create_null();
    test_csv_writer_create_from_file();
    test_csv_writer_create_from_file_null();
    test_csv_writer_write_field();
    test_csv_writer_write_field_null();
    test_csv_writer_write_row();
    test_csv_writer_write_row_null();
    test_csv_writer_write_row_va();
    test_csv_writer_flush();
    test_csv_writer_has_error();
    test_csv_writer_strerror();

    return 0;
}
