#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/csv.h"

static const char* test_csv_file = "/tmp/test_csv_unit.csv";

static void create_test_csv_file(void) {
    FILE *fp = fopen(test_csv_file, "w");
    if (fp) {
        fprintf(fp, "name,age,city\n");
        fprintf(fp, "Alice,30,New York\n");
        fprintf(fp, "Bob,25,Los Angeles\n");
        fprintf(fp, "\"Charlie, Jr.\",35,\"San Francisco, CA\"\n");
        fclose(fp);
    }
}

void test_csv_default_config() {
    TEST(Csv_DefaultConfig);
    csv_config_t config = csv_default_config();
    
    EXPECT_EQ(config.delimiter, ',');
    EXPECT_EQ(config.quote, '"');
    EXPECT_EQ(config.escape, '\\');
    EXPECT_TRUE(config.trim_whitespace == false);
    EXPECT_TRUE(config.ignore_empty_lines == true);
    EXPECT_TRUE(config.header_row == false);
}

void test_csv_create_free() {
    TEST(Csv_CreateFree);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    
    EXPECT_TRUE(csv != NULL);
    EXPECT_EQ(error, CSV_OK);
    
    csv_free(csv);
}

void test_csv_create_with_config() {
    TEST(Csv_CreateWithConfig);
    csv_config_t config = csv_default_config();
    config.delimiter = ';';
    config.trim_whitespace = true;
    
    csv_error_t error;
    csv_t* csv = csv_create(&config, &error);
    
    EXPECT_TRUE(csv != NULL);
    EXPECT_EQ(error, CSV_OK);
    EXPECT_EQ(csv->config.delimiter, ';');
    EXPECT_TRUE(csv->config.trim_whitespace == true);
    
    csv_free(csv);
}

void test_csv_add_row() {
    TEST(Csv_AddRow);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b", "c"};
    bool result = csv_add_row(csv, fields, 3, &error);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(csv_get_rows(csv), (size_t)1);
    EXPECT_EQ(csv_get_cols(csv), (size_t)3);
    
    csv_free(csv);
}

void test_csv_add_multiple_rows() {
    TEST(Csv_AddMultipleRows);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* row1[] = {"name", "age", "city"};
    const char* row2[] = {"Alice", "30", "NYC"};
    const char* row3[] = {"Bob", "25", "LA"};
    
    EXPECT_TRUE(csv_add_row(csv, row1, 3, &error));
    EXPECT_TRUE(csv_add_row(csv, row2, 3, &error));
    EXPECT_TRUE(csv_add_row(csv, row3, 3, &error));
    
    EXPECT_EQ(csv_get_rows(csv), (size_t)3);
    EXPECT_EQ(csv_get_cols(csv), (size_t)3);
    
    csv_free(csv);
}

void test_csv_get() {
    TEST(Csv_Get);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b", "c"};
    csv_add_row(csv, fields, 3, &error);
    
    const char* val = csv_get(csv, 0, 1, &error);
    EXPECT_TRUE(val != NULL);
    EXPECT_TRUE(strcmp(val, "b") == 0);
    
    val = csv_get(csv, 0, 0, &error);
    EXPECT_TRUE(strcmp(val, "a") == 0);
    
    csv_free(csv);
}

void test_csv_set() {
    TEST(Csv_Set);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b", "c"};
    csv_add_row(csv, fields, 3, &error);
    
    EXPECT_TRUE(csv_set(csv, 0, 1, "modified", &error));
    
    const char* val = csv_get(csv, 0, 1, &error);
    EXPECT_TRUE(strcmp(val, "modified") == 0);
    
    csv_free(csv);
}

void test_csv_parse_line() {
    TEST(Csv_ParseLine);
    csv_error_t error;
    
    csv_row_t* row = csv_parse_line("a,b,c", NULL, &error);
    EXPECT_TRUE(row != NULL);
    EXPECT_EQ(row->count, (size_t)3);
    EXPECT_TRUE(strcmp(row->fields[0], "a") == 0);
    EXPECT_TRUE(strcmp(row->fields[1], "b") == 0);
    EXPECT_TRUE(strcmp(row->fields[2], "c") == 0);
    
    csv_row_free(row);
}

void test_csv_parse_line_quoted() {
    TEST(Csv_ParseLineQuoted);
    csv_error_t error;
    
    csv_row_t* row = csv_parse_line("\"a,b\",\"c\"", NULL, &error);
    EXPECT_TRUE(row != NULL);
    EXPECT_EQ(row->count, (size_t)2);
    EXPECT_TRUE(strcmp(row->fields[0], "a,b") == 0);
    EXPECT_TRUE(strcmp(row->fields[1], "c") == 0);
    
    csv_row_free(row);
}

void test_csv_parse_line_empty_fields() {
    TEST(Csv_ParseLineEmptyFields);
    csv_error_t error;
    
    csv_row_t* row = csv_parse_line("a,,c", NULL, &error);
    EXPECT_TRUE(row != NULL);
    EXPECT_EQ(row->count, (size_t)3);
    EXPECT_TRUE(strcmp(row->fields[0], "a") == 0);
    EXPECT_TRUE(strcmp(row->fields[1], "") == 0);
    EXPECT_TRUE(strcmp(row->fields[2], "c") == 0);
    
    csv_row_free(row);
}

void test_csv_trim_whitespace() {
    TEST(Csv_TrimWhitespace);
    csv_config_t config = csv_default_config();
    config.trim_whitespace = true;
    
    csv_error_t error;
    csv_row_t* row = csv_parse_line("  a  ,  b  ,  c  ", &config, &error);
    EXPECT_TRUE(row != NULL);
    EXPECT_EQ(row->count, (size_t)3);
    EXPECT_TRUE(strcmp(row->fields[0], "a") == 0);
    EXPECT_TRUE(strcmp(row->fields[1], "b") == 0);
    EXPECT_TRUE(strcmp(row->fields[2], "c") == 0);
    
    csv_row_free(row);
}

void test_csv_different_delimiter() {
    TEST(Csv_DifferentDelimiter);
    csv_config_t config = csv_default_config();
    config.delimiter = ';';
    
    csv_error_t error;
    csv_row_t* row = csv_parse_line("a;b;c", &config, &error);
    EXPECT_TRUE(row != NULL);
    EXPECT_EQ(row->count, (size_t)3);
    EXPECT_TRUE(strcmp(row->fields[0], "a") == 0);
    EXPECT_TRUE(strcmp(row->fields[1], "b") == 0);
    EXPECT_TRUE(strcmp(row->fields[2], "c") == 0);
    
    csv_row_free(row);
}

void test_csv_load() {
    TEST(Csv_Load);
    create_test_csv_file();
    
    csv_error_t error;
    csv_t* csv = csv_load(test_csv_file, NULL, &error);
    
    EXPECT_TRUE(csv != NULL);
    EXPECT_EQ(error, CSV_OK);
    EXPECT_EQ(csv_get_rows(csv), (size_t)4);
    EXPECT_EQ(csv_get_cols(csv), (size_t)3);
    
    EXPECT_TRUE(strcmp(csv_get(csv, 0, 0, &error), "name") == 0);
    EXPECT_TRUE(strcmp(csv_get(csv, 1, 0, &error), "Alice") == 0);
    EXPECT_TRUE(strcmp(csv_get(csv, 2, 1, &error), "25") == 0);
    
    csv_free(csv);
    remove(test_csv_file);
}

void test_csv_save() {
    TEST(Csv_Save);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* row1[] = {"name", "age"};
    const char* row2[] = {"Alice", "30"};
    csv_add_row(csv, row1, 2, &error);
    csv_add_row(csv, row2, 2, &error);
    
    EXPECT_TRUE(csv_save(csv, test_csv_file, &error));
    
    csv_free(csv);
    
    csv = csv_load(test_csv_file, NULL, &error);
    EXPECT_TRUE(csv != NULL);
    EXPECT_EQ(csv_get_rows(csv), (size_t)2);
    EXPECT_TRUE(strcmp(csv_get(csv, 0, 0, &error), "name") == 0);
    EXPECT_TRUE(strcmp(csv_get(csv, 1, 0, &error), "Alice") == 0);
    
    csv_free(csv);
    remove(test_csv_file);
}

void test_csv_out_of_bounds() {
    TEST(Csv_OutOfBounds);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b"};
    csv_add_row(csv, fields, 2, &error);
    
    const char* val = csv_get(csv, 10, 0, &error);
    EXPECT_TRUE(val == NULL);
    EXPECT_EQ(error, CSV_ERROR_OUT_OF_BOUNDS);
    
    val = csv_get(csv, 0, 10, &error);
    EXPECT_TRUE(val == NULL);
    EXPECT_EQ(error, CSV_ERROR_OUT_OF_BOUNDS);
    
    csv_free(csv);
}

void test_csv_column_mismatch() {
    TEST(Csv_ColumnMismatch);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* row1[] = {"a", "b", "c"};
    const char* row2[] = {"x", "y"};
    
    EXPECT_TRUE(csv_add_row(csv, row1, 3, &error));
    EXPECT_TRUE(csv_add_row(csv, row2, 2, &error) == false);
    EXPECT_EQ(error, CSV_ERROR_COLUMN_MISMATCH);
    
    csv_free(csv);
}

void test_csv_null_params() {
    TEST(Csv_NullParams);
    csv_error_t error;
    
    csv_t* csv = csv_create(NULL, NULL);
    EXPECT_TRUE(csv != NULL);
    csv_free(csv);
    
    EXPECT_TRUE(csv_parse_line(NULL, NULL, &error) == NULL);
    EXPECT_EQ(error, CSV_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(csv_load(NULL, NULL, &error) == NULL);
    EXPECT_EQ(error, CSV_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(csv_save(NULL, "test.csv", &error) == false);
    EXPECT_EQ(error, CSV_ERROR_INVALID_PARAM);
    
    csv_free(NULL);
}

void test_csv_strerror() {
    TEST(Csv_Strerror);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_OK), "Success") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_FILE_OPEN), "Failed to open file") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_PARSE), "Parse error") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_INVALID_PARAM), "Invalid parameter") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_MEMORY_ALLOC), "Memory allocation failed") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_COLUMN_MISMATCH), "Column count mismatch") == 0);
    EXPECT_TRUE(strcmp(csv_strerror(CSV_ERROR_OUT_OF_BOUNDS), "Index out of bounds") == 0);
}

void test_csv_error_file() {
    TEST(Csv_ErrorFile);
    csv_error_t error;
    
    csv_t* csv = csv_load("/nonexistent/file.csv", NULL, &error);
    EXPECT_TRUE(csv == NULL);
    EXPECT_EQ(error, CSV_ERROR_FILE_OPEN);
}

int main() {
    test_csv_default_config();
    test_csv_create_free();
    test_csv_create_with_config();
    test_csv_add_row();
    test_csv_add_multiple_rows();
    test_csv_get();
    test_csv_set();
    test_csv_parse_line();
    test_csv_parse_line_quoted();
    test_csv_parse_line_empty_fields();
    test_csv_trim_whitespace();
    test_csv_different_delimiter();
    test_csv_load();
    test_csv_save();
    test_csv_out_of_bounds();
    test_csv_column_mismatch();
    test_csv_null_params();
    test_csv_strerror();
    test_csv_error_file();

    return 0;
}
