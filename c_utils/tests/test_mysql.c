#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mysql_wrapper.h"

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

TEST(error_string) {
    const char* str = mysql_wrap_error_string(MYSQL_WRAP_OK);
    ASSERT_TRUE(str != NULL);
    ASSERT_TRUE(strlen(str) > 0);
    
    str = mysql_wrap_error_string(MYSQL_WRAP_ERR_NULL_PTR);
    ASSERT_TRUE(str != NULL);
    
    str = mysql_wrap_error_string(MYSQL_WRAP_ERR_CONNECT);
    ASSERT_TRUE(str != NULL);
    
    str = mysql_wrap_error_string((mysql_wrap_error_t)-999);
    ASSERT_TRUE(str != NULL);
}

TEST(config_init) {
    mysql_config_t config;
    mysql_config_init(&config);
    
    ASSERT_TRUE(strcmp(config.host, "localhost") == 0);
    ASSERT_EQ(config.port, 3306);
    ASSERT_EQ(config.user[0], '\0');
    ASSERT_EQ(config.password[0], '\0');
    ASSERT_EQ(config.database[0], '\0');
    ASSERT_TRUE(strlen(config.charset) > 0);
    ASSERT_EQ(config.timeout, 30);
    ASSERT_EQ(config.auto_reconnect, true);
}

TEST(config_setters) {
    mysql_config_t config;
    mysql_config_init(&config);
    
    mysql_config_set_host(&config, "localhost");
    ASSERT_TRUE(strcmp(config.host, "localhost") == 0);
    
    mysql_config_set_port(&config, 3307);
    ASSERT_EQ(config.port, 3307);
    
    mysql_config_set_user(&config, "testuser");
    ASSERT_TRUE(strcmp(config.user, "testuser") == 0);
    
    mysql_config_set_password(&config, "testpass");
    ASSERT_TRUE(strcmp(config.password, "testpass") == 0);
    
    mysql_config_set_database(&config, "testdb");
    ASSERT_TRUE(strcmp(config.database, "testdb") == 0);
    
    mysql_config_set_charset(&config, "utf8mb4");
    ASSERT_TRUE(strcmp(config.charset, "utf8mb4") == 0);
}

TEST(connect_null_config) {
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect(NULL, &error);
    ASSERT_TRUE(conn == NULL);
    ASSERT_EQ(error, MYSQL_WRAP_ERR_NULL_PTR);
}

TEST(connect_simple_null_params) {
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect_simple(NULL, 0, NULL, NULL, NULL, &error);
    ASSERT_TRUE(conn == NULL);
    ASSERT_EQ(error, MYSQL_WRAP_ERR_CONNECT);
}

TEST(disconnect_null) {
    mysql_wrap_disconnect(NULL);
}

TEST(is_connected_null) {
    ASSERT_TRUE(!mysql_wrap_is_connected(NULL));
}

TEST(error_null_conn) {
    const char* err = mysql_wrap_error(NULL);
    ASSERT_TRUE(err != NULL);
    
    unsigned int err_code = mysql_wrap_errno(NULL);
    ASSERT_EQ(err_code, 0);
}

TEST(query_null_conn) {
    mysql_wrap_result_t* result = mysql_wrap_query(NULL, "SELECT 1");
    ASSERT_TRUE(result == NULL);
}

TEST(query_null_sql) {
    mysql_config_t config;
    mysql_config_init(&config);
    mysql_config_set_host(&config, "localhost");
    
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect(&config, &error);
    
    if (conn) {
        mysql_wrap_result_t* result = mysql_wrap_query(conn, NULL);
        ASSERT_TRUE(result == NULL);
        mysql_wrap_disconnect(conn);
    } else {
        printf("    (跳过: 无法连接数据库)\n");
    }
}

TEST(execute_null_conn) {
    int result = mysql_wrap_execute(NULL, "CREATE TABLE test (id INT)");
    ASSERT_EQ(result, -1);
}

TEST(result_free_null) {
    mysql_wrap_result_free(NULL);
}

TEST(result_num_rows_null) {
    ASSERT_EQ(mysql_wrap_result_num_rows(NULL), 0);
}

TEST(result_num_fields_null) {
    ASSERT_EQ(mysql_wrap_result_num_fields(NULL), 0);
}

TEST(result_field_name_null) {
    const char* name = mysql_wrap_result_field_name(NULL, 0);
    ASSERT_TRUE(name == NULL);
}

TEST(result_next_null) {
    ASSERT_TRUE(!mysql_wrap_result_next(NULL));
}

TEST(get_int_null) {
    ASSERT_EQ(mysql_wrap_get_int(NULL, 0), 0);
}

TEST(get_int64_null) {
    ASSERT_EQ(mysql_wrap_get_int64(NULL, 0), 0);
}

TEST(get_double_null) {
    ASSERT_EQ(mysql_wrap_get_double(NULL, 0), 0.0);
}

TEST(get_string_null) {
    ASSERT_TRUE(mysql_wrap_get_string(NULL, 0) == NULL);
}

TEST(is_null_null) {
    ASSERT_TRUE(mysql_wrap_is_null(NULL, 0));
}

TEST(get_field_index_null) {
    ASSERT_EQ(mysql_wrap_get_field_index(NULL, "id"), -1);
}

TEST(get_int_by_name_null) {
    ASSERT_EQ(mysql_wrap_get_int_by_name(NULL, "id"), 0);
}

TEST(get_int64_by_name_null) {
    ASSERT_EQ(mysql_wrap_get_int64_by_name(NULL, "id"), 0);
}

TEST(get_double_by_name_null) {
    ASSERT_EQ(mysql_wrap_get_double_by_name(NULL, "value"), 0.0);
}

TEST(get_string_by_name_null) {
    ASSERT_TRUE(mysql_wrap_get_string_by_name(NULL, "name") == NULL);
}

TEST(transaction_null_conn) {
    mysql_wrap_error_t err = mysql_wrap_begin_transaction(NULL);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
    
    err = mysql_wrap_commit(NULL);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
    
    err = mysql_wrap_rollback(NULL);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
}

TEST(pool_create_null_config) {
    mysql_wrap_pool_t* pool = mysql_wrap_pool_create(NULL, 1, 5);
    ASSERT_TRUE(pool == NULL);
}

TEST(pool_destroy_null) {
    mysql_wrap_pool_destroy(NULL);
}

TEST(pool_get_null) {
    mysql_wrap_conn_t* conn = mysql_wrap_pool_get(NULL);
    ASSERT_TRUE(conn == NULL);
}

TEST(pool_release_null) {
    mysql_wrap_pool_release(NULL, NULL);
}

TEST(pool_size_null) {
    ASSERT_EQ(mysql_wrap_pool_size(NULL), 0);
}

TEST(pool_available_null) {
    ASSERT_EQ(mysql_wrap_pool_available(NULL), 0);
}

TEST(insert_null_params) {
    mysql_wrap_error_t err = mysql_wrap_insert(NULL, NULL, NULL, NULL, 0);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
}

TEST(update_null_params) {
    mysql_wrap_error_t err = mysql_wrap_update(NULL, NULL, NULL, NULL, 0, NULL);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
}

TEST(delete_null_params) {
    mysql_wrap_error_t err = mysql_wrap_delete(NULL, NULL, NULL);
    ASSERT_EQ(err, MYSQL_WRAP_ERR_NULL_PTR);
}

TEST(escape_string_null) {
    char* escaped = mysql_wrap_escape_string(NULL, "test");
    ASSERT_TRUE(escaped == NULL);
}

TEST(free_string_null) {
    mysql_wrap_free_string(NULL);
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    MySQL封装库测试用例                         \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("错误处理测试:\n");
    RUN_TEST(error_string);
    
    printf("\n配置测试:\n");
    RUN_TEST(config_init);
    RUN_TEST(config_setters);
    
    printf("\n空指针测试:\n");
    RUN_TEST(connect_null_config);
    RUN_TEST(connect_simple_null_params);
    RUN_TEST(disconnect_null);
    RUN_TEST(is_connected_null);
    RUN_TEST(error_null_conn);
    RUN_TEST(query_null_conn);
    RUN_TEST(query_null_sql);
    RUN_TEST(execute_null_conn);
    RUN_TEST(result_free_null);
    RUN_TEST(result_num_rows_null);
    RUN_TEST(result_num_fields_null);
    RUN_TEST(result_field_name_null);
    RUN_TEST(result_next_null);
    RUN_TEST(get_int_null);
    RUN_TEST(get_int64_null);
    RUN_TEST(get_double_null);
    RUN_TEST(get_string_null);
    RUN_TEST(is_null_null);
    RUN_TEST(get_field_index_null);
    RUN_TEST(get_int_by_name_null);
    RUN_TEST(get_int64_by_name_null);
    RUN_TEST(get_double_by_name_null);
    RUN_TEST(get_string_by_name_null);
    RUN_TEST(transaction_null_conn);
    RUN_TEST(pool_create_null_config);
    RUN_TEST(pool_destroy_null);
    RUN_TEST(pool_get_null);
    RUN_TEST(pool_release_null);
    RUN_TEST(pool_size_null);
    RUN_TEST(pool_available_null);
    RUN_TEST(insert_null_params);
    RUN_TEST(update_null_params);
    RUN_TEST(delete_null_params);
    RUN_TEST(escape_string_null);
    RUN_TEST(free_string_null);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("测试结果: 通过 %d, 失败 %d\n", tests_passed, tests_failed);
    printf("═══════════════════════════════════════════════════════════════\n\n");
    printf("注意: 完整功能测试需要运行中的MySQL服务器。\n\n");
    
    return tests_failed > 0 ? 1 : 0;
}
