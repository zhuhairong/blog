#ifndef C_UTILS_MYSQL_WRAPPER_H
#define C_UTILS_MYSQL_WRAPPER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MYSQL_WRAP_OK = 0,
    MYSQL_WRAP_ERR_NULL_PTR = -1,
    MYSQL_WRAP_ERR_MEMORY = -2,
    MYSQL_WRAP_ERR_CONNECT = -3,
    MYSQL_WRAP_ERR_QUERY = -4,
    MYSQL_WRAP_ERR_NO_RESULT = -5,
    MYSQL_WRAP_ERR_PARAM = -6,
    MYSQL_WRAP_ERR_TRANSACTION = -7,
    MYSQL_WRAP_ERR_POOL = -8,
    MYSQL_WRAP_ERR_NOT_CONNECTED = -9,
} mysql_wrap_error_t;

typedef struct {
    char host[256];
    unsigned int port;
    char user[64];
    char password[128];
    char database[64];
    char charset[32];
    unsigned int timeout;
    bool auto_reconnect;
} mysql_config_t;

typedef struct mysql_wrap_conn mysql_wrap_conn_t;
typedef struct mysql_wrap_result mysql_wrap_result_t;
typedef struct mysql_wrap_pool mysql_wrap_pool_t;

const char* mysql_wrap_error_string(mysql_wrap_error_t error);

void mysql_config_init(mysql_config_t* config);
void mysql_config_set_host(mysql_config_t* config, const char* host);
void mysql_config_set_port(mysql_config_t* config, unsigned int port);
void mysql_config_set_user(mysql_config_t* config, const char* user);
void mysql_config_set_password(mysql_config_t* config, const char* password);
void mysql_config_set_database(mysql_config_t* config, const char* database);
void mysql_config_set_charset(mysql_config_t* config, const char* charset);

mysql_wrap_conn_t* mysql_wrap_connect(const mysql_config_t* config, mysql_wrap_error_t* error);
mysql_wrap_conn_t* mysql_wrap_connect_simple(const char* host, unsigned int port, 
                                    const char* user, const char* password,
                                    const char* database, mysql_wrap_error_t* error);
void mysql_wrap_disconnect(mysql_wrap_conn_t* conn);
bool mysql_wrap_is_connected(mysql_wrap_conn_t* conn);
const char* mysql_wrap_error(mysql_wrap_conn_t* conn);
unsigned int mysql_wrap_errno(mysql_wrap_conn_t* conn);

mysql_wrap_result_t* mysql_wrap_query(mysql_wrap_conn_t* conn, const char* sql);
int mysql_wrap_execute(mysql_wrap_conn_t* conn, const char* sql);
uint64_t mysql_wrap_affected_rows(mysql_wrap_conn_t* conn);
uint64_t mysql_wrap_last_insert_id(mysql_wrap_conn_t* conn);

void mysql_wrap_result_free(mysql_wrap_result_t* result);
uint64_t mysql_wrap_result_num_rows(mysql_wrap_result_t* result);
unsigned int mysql_wrap_result_num_fields(mysql_wrap_result_t* result);
const char* mysql_wrap_result_field_name(mysql_wrap_result_t* result, unsigned int index);
bool mysql_wrap_result_next(mysql_wrap_result_t* result);

int mysql_wrap_get_int(mysql_wrap_result_t* result, unsigned int index);
int64_t mysql_wrap_get_int64(mysql_wrap_result_t* result, unsigned int index);
double mysql_wrap_get_double(mysql_wrap_result_t* result, unsigned int index);
const char* mysql_wrap_get_string(mysql_wrap_result_t* result, unsigned int index);
bool mysql_wrap_is_null(mysql_wrap_result_t* result, unsigned int index);

int mysql_wrap_get_int_by_name(mysql_wrap_result_t* result, const char* name);
int64_t mysql_wrap_get_int64_by_name(mysql_wrap_result_t* result, const char* name);
double mysql_wrap_get_double_by_name(mysql_wrap_result_t* result, const char* name);
const char* mysql_wrap_get_string_by_name(mysql_wrap_result_t* result, const char* name);
int mysql_wrap_get_field_index(mysql_wrap_result_t* result, const char* name);

mysql_wrap_error_t mysql_wrap_begin_transaction(mysql_wrap_conn_t* conn);
mysql_wrap_error_t mysql_wrap_commit(mysql_wrap_conn_t* conn);
mysql_wrap_error_t mysql_wrap_rollback(mysql_wrap_conn_t* conn);

mysql_wrap_pool_t* mysql_wrap_pool_create(const mysql_config_t* config, unsigned int min_size, unsigned int max_size);
void mysql_wrap_pool_destroy(mysql_wrap_pool_t* pool);
mysql_wrap_conn_t* mysql_wrap_pool_get(mysql_wrap_pool_t* pool);
void mysql_wrap_pool_release(mysql_wrap_pool_t* pool, mysql_wrap_conn_t* conn);
unsigned int mysql_wrap_pool_size(mysql_wrap_pool_t* pool);
unsigned int mysql_wrap_pool_available(mysql_wrap_pool_t* pool);

mysql_wrap_error_t mysql_wrap_insert(mysql_wrap_conn_t* conn, const char* table, 
                                 const char** fields, const char** values, unsigned int count);
mysql_wrap_error_t mysql_wrap_update(mysql_wrap_conn_t* conn, const char* table, 
                                 const char** fields, const char** values, unsigned int count, 
                                 const char* where);
mysql_wrap_error_t mysql_wrap_delete(mysql_wrap_conn_t* conn, const char* table, const char* where);

char* mysql_wrap_escape_string(mysql_wrap_conn_t* conn, const char* str);
void mysql_wrap_free_string(char* str);

#endif
