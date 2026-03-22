#include "mysql_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef C_UTILS_HAS_MYSQL
#include <mysql/mysql.h>
#endif

struct mysql_wrap_conn {
#ifdef C_UTILS_HAS_MYSQL
    MYSQL* mysql;
#endif
    bool connected;
    char error_msg[512];
    unsigned int error_code;
};

struct mysql_wrap_result {
#ifdef C_UTILS_HAS_MYSQL
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL_FIELD* fields;
#endif
    unsigned int num_fields;
    uint64_t num_rows;
    mysql_wrap_conn_t* conn;
};

struct mysql_wrap_pool {
    mysql_config_t config;
    mysql_wrap_conn_t** connections;
    bool* available;
    unsigned int min_size;
    unsigned int max_size;
    unsigned int current_size;
};

const char* mysql_wrap_error_string(mysql_wrap_error_t error) {
    switch (error) {
        case MYSQL_WRAP_OK: return "Success";
        case MYSQL_WRAP_ERR_NULL_PTR: return "Null pointer";
        case MYSQL_WRAP_ERR_MEMORY: return "Memory allocation failed";
        case MYSQL_WRAP_ERR_CONNECT: return "Connection failed";
        case MYSQL_WRAP_ERR_QUERY: return "Query execution failed";
        case MYSQL_WRAP_ERR_NO_RESULT: return "No result set";
        case MYSQL_WRAP_ERR_PARAM: return "Invalid parameter";
        case MYSQL_WRAP_ERR_TRANSACTION: return "Transaction error";
        case MYSQL_WRAP_ERR_POOL: return "Connection pool error";
        case MYSQL_WRAP_ERR_NOT_CONNECTED: return "Not connected";
        default: return "Unknown error";
    }
}

void mysql_config_init(mysql_config_t* config) {
    if (!config) return;
    memset(config, 0, sizeof(mysql_config_t));
    strcpy(config->host, "localhost");
    config->port = 3306;
    strcpy(config->charset, "utf8mb4");
    config->timeout = 30;
    config->auto_reconnect = true;
}

void mysql_config_set_host(mysql_config_t* config, const char* host) {
    if (!config || !host) return;
    strncpy(config->host, host, sizeof(config->host) - 1);
}

void mysql_config_set_port(mysql_config_t* config, unsigned int port) {
    if (!config) return;
    config->port = port;
}

void mysql_config_set_user(mysql_config_t* config, const char* user) {
    if (!config || !user) return;
    strncpy(config->user, user, sizeof(config->user) - 1);
}

void mysql_config_set_password(mysql_config_t* config, const char* password) {
    if (!config || !password) return;
    strncpy(config->password, password, sizeof(config->password) - 1);
}

void mysql_config_set_database(mysql_config_t* config, const char* database) {
    if (!config || !database) return;
    strncpy(config->database, database, sizeof(config->database) - 1);
}

void mysql_config_set_charset(mysql_config_t* config, const char* charset) {
    if (!config || !charset) return;
    strncpy(config->charset, charset, sizeof(config->charset) - 1);
}

#ifdef C_UTILS_HAS_MYSQL

mysql_wrap_conn_t* mysql_wrap_connect(const mysql_config_t* config, mysql_wrap_error_t* error) {
    if (!config) {
        if (error) *error = MYSQL_WRAP_ERR_NULL_PTR;
        return NULL;
    }
    
    mysql_wrap_conn_t* conn = (mysql_wrap_conn_t*)calloc(1, sizeof(mysql_wrap_conn_t));
    if (!conn) {
        if (error) *error = MYSQL_WRAP_ERR_MEMORY;
        return NULL;
    }
    
    conn->mysql = mysql_init(NULL);
    if (!conn->mysql) {
        if (error) *error = MYSQL_WRAP_ERR_MEMORY;
        free(conn);
        return NULL;
    }
    
    unsigned int timeout = config->timeout;
    mysql_options(conn->mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    mysql_options(conn->mysql, MYSQL_OPT_READ_TIMEOUT, &timeout);
    mysql_options(conn->mysql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    
    if (config->auto_reconnect) {
        bool reconnect = true;
        mysql_options(conn->mysql, MYSQL_OPT_RECONNECT, &reconnect);
    }
    
    if (!mysql_real_connect(conn->mysql, config->host, config->user, 
                            config->password, config->database[0] ? config->database : NULL,
                            config->port, NULL, 0)) {
        if (error) *error = MYSQL_WRAP_ERR_CONNECT;
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        mysql_close(conn->mysql);
        free(conn);
        return NULL;
    }
    
    if (config->charset[0] && mysql_set_character_set(conn->mysql, config->charset) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
    }
    
    conn->connected = true;
    if (error) *error = MYSQL_WRAP_OK;
    return conn;
}

void mysql_wrap_disconnect(mysql_wrap_conn_t* conn) {
    if (!conn) return;
    
    if (conn->mysql) {
        mysql_close(conn->mysql);
        conn->mysql = NULL;
    }
    conn->connected = false;
    free(conn);
}

const char* mysql_wrap_error(mysql_wrap_conn_t* conn) {
    if (!conn) return "Null connection";
    if (conn->error_msg[0]) return conn->error_msg;
    if (conn->mysql) return mysql_error(conn->mysql);
    return "Unknown error";
}

unsigned int mysql_wrap_errno(mysql_wrap_conn_t* conn) {
    if (!conn) return 0;
    if (conn->error_code) return conn->error_code;
    if (conn->mysql) return mysql_errno(conn->mysql);
    return 0;
}

mysql_wrap_result_t* mysql_wrap_query(mysql_wrap_conn_t* conn, const char* sql) {
    if (!conn || !sql) return NULL;
    if (!conn->connected) {
        strncpy(conn->error_msg, "Not connected", sizeof(conn->error_msg) - 1);
        conn->error_code = CR_SERVER_GONE_ERROR;
        return NULL;
    }
    
    if (mysql_query(conn->mysql, sql) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        return NULL;
    }
    
    MYSQL_RES* result = mysql_store_result(conn->mysql);
    if (!result) {
        if (mysql_field_count(conn->mysql) > 0) {
            strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
            conn->error_code = mysql_errno(conn->mysql);
            return NULL;
        }
        return NULL;
    }
    
    mysql_wrap_result_t* res = (mysql_wrap_result_t*)calloc(1, sizeof(mysql_wrap_result_t));
    if (!res) {
        mysql_free_result(result);
        return NULL;
    }
    
    res->result = result;
    res->conn = conn;
    res->num_rows = mysql_num_rows(result);
    res->num_fields = mysql_num_fields(result);
    res->fields = mysql_fetch_fields(result);
    
    return res;
}

int mysql_wrap_execute(mysql_wrap_conn_t* conn, const char* sql) {
    if (!conn || !sql) return -1;
    if (!conn->connected) {
        strncpy(conn->error_msg, "Not connected", sizeof(conn->error_msg) - 1);
        conn->error_code = CR_SERVER_GONE_ERROR;
        return -1;
    }
    
    if (mysql_query(conn->mysql, sql) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        return -1;
    }
    
    return 0;
}

uint64_t mysql_wrap_affected_rows(mysql_wrap_conn_t* conn) {
    if (!conn || !conn->mysql) return 0;
    return mysql_affected_rows(conn->mysql);
}

uint64_t mysql_wrap_last_insert_id(mysql_wrap_conn_t* conn) {
    if (!conn || !conn->mysql) return 0;
    return mysql_insert_id(conn->mysql);
}

void mysql_wrap_result_free(mysql_wrap_result_t* result) {
    if (!result) return;
    if (result->result) {
        mysql_free_result(result->result);
    }
    free(result);
}

bool mysql_wrap_result_next(mysql_wrap_result_t* result) {
    if (!result || !result->result) return false;
    result->row = mysql_fetch_row(result->result);
    return result->row != NULL;
}

int mysql_wrap_get_int(mysql_wrap_result_t* result, unsigned int index) {
    if (!result || !result->row || index >= result->num_fields) return 0;
    if (!result->row[index]) return 0;
    return atoi(result->row[index]);
}

int64_t mysql_wrap_get_int64(mysql_wrap_result_t* result, unsigned int index) {
    if (!result || !result->row || index >= result->num_fields) return 0;
    if (!result->row[index]) return 0;
    return atoll(result->row[index]);
}

double mysql_wrap_get_double(mysql_wrap_result_t* result, unsigned int index) {
    if (!result || !result->row || index >= result->num_fields) return 0.0;
    if (!result->row[index]) return 0.0;
    return atof(result->row[index]);
}

const char* mysql_wrap_get_string(mysql_wrap_result_t* result, unsigned int index) {
    if (!result || !result->row || index >= result->num_fields) return NULL;
    return result->row[index];
}

bool mysql_wrap_is_null(mysql_wrap_result_t* result, unsigned int index) {
    if (!result || !result->row || index >= result->num_fields) return true;
    return result->row[index] == NULL;
}

int mysql_wrap_get_field_index(mysql_wrap_result_t* result, const char* name) {
    if (!result || !result->fields || !name) return -1;
    
    for (unsigned int i = 0; i < result->num_fields; i++) {
        if (strcmp(result->fields[i].name, name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

mysql_wrap_error_t mysql_wrap_begin_transaction(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    if (!conn->connected) return MYSQL_WRAP_ERR_NOT_CONNECTED;
    
    if (mysql_autocommit(conn->mysql, 0) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        return MYSQL_WRAP_ERR_TRANSACTION;
    }
    
    return MYSQL_WRAP_OK;
}

mysql_wrap_error_t mysql_wrap_commit(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    if (!conn->connected) return MYSQL_WRAP_ERR_NOT_CONNECTED;
    
    if (mysql_commit(conn->mysql) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        return MYSQL_WRAP_ERR_TRANSACTION;
    }
    
    mysql_autocommit(conn->mysql, 1);
    return MYSQL_WRAP_OK;
}

mysql_wrap_error_t mysql_wrap_rollback(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    if (!conn->connected) return MYSQL_WRAP_ERR_NOT_CONNECTED;
    
    if (mysql_rollback(conn->mysql) != 0) {
        strncpy(conn->error_msg, mysql_error(conn->mysql), sizeof(conn->error_msg) - 1);
        conn->error_code = mysql_errno(conn->mysql);
        return MYSQL_WRAP_ERR_TRANSACTION;
    }
    
    mysql_autocommit(conn->mysql, 1);
    return MYSQL_WRAP_OK;
}

char* mysql_wrap_escape_string(mysql_wrap_conn_t* conn, const char* str) {
    if (!conn || !conn->mysql || !str) return NULL;
    
    size_t len = strlen(str);
    size_t buf_size = len * 2 + 1;
    char* escaped = (char*)malloc(buf_size);
    if (!escaped) return NULL;
    
    mysql_real_escape_string(conn->mysql, escaped, str, len);
    return escaped;
}

#else

mysql_wrap_conn_t* mysql_wrap_connect(const mysql_config_t* config, mysql_wrap_error_t* error) {
    if (!config) {
        if (error) *error = MYSQL_WRAP_ERR_NULL_PTR;
        return NULL;
    }
    if (error) *error = MYSQL_WRAP_ERR_CONNECT;
    return NULL;
}

void mysql_wrap_disconnect(mysql_wrap_conn_t* conn) {
    if (!conn) return;
    free(conn);
}

const char* mysql_wrap_error(mysql_wrap_conn_t* conn) {
    if (!conn) return "Null connection";
    if (conn->error_msg[0]) return conn->error_msg;
    return "MySQL library not available";
}

unsigned int mysql_wrap_errno(mysql_wrap_conn_t* conn) {
    if (!conn) return 0;
    return conn->error_code;
}

mysql_wrap_result_t* mysql_wrap_query(mysql_wrap_conn_t* conn, const char* sql) {
    (void)sql;
    if (!conn) return NULL;
    strncpy(conn->error_msg, "MySQL library not available", sizeof(conn->error_msg) - 1);
    return NULL;
}

int mysql_wrap_execute(mysql_wrap_conn_t* conn, const char* sql) {
    (void)sql;
    if (!conn) return -1;
    strncpy(conn->error_msg, "MySQL library not available", sizeof(conn->error_msg) - 1);
    return -1;
}

uint64_t mysql_wrap_affected_rows(mysql_wrap_conn_t* conn) {
    (void)conn;
    return 0;
}

uint64_t mysql_wrap_last_insert_id(mysql_wrap_conn_t* conn) {
    (void)conn;
    return 0;
}

void mysql_wrap_result_free(mysql_wrap_result_t* result) {
    if (!result) return;
    free(result);
}

bool mysql_wrap_result_next(mysql_wrap_result_t* result) {
    (void)result;
    return false;
}

int mysql_wrap_get_int(mysql_wrap_result_t* result, unsigned int index) {
    (void)result; (void)index;
    return 0;
}

int64_t mysql_wrap_get_int64(mysql_wrap_result_t* result, unsigned int index) {
    (void)result; (void)index;
    return 0;
}

double mysql_wrap_get_double(mysql_wrap_result_t* result, unsigned int index) {
    (void)result; (void)index;
    return 0.0;
}

const char* mysql_wrap_get_string(mysql_wrap_result_t* result, unsigned int index) {
    (void)result; (void)index;
    return NULL;
}

bool mysql_wrap_is_null(mysql_wrap_result_t* result, unsigned int index) {
    (void)result; (void)index;
    return true;
}

int mysql_wrap_get_field_index(mysql_wrap_result_t* result, const char* name) {
    (void)result; (void)name;
    return -1;
}

mysql_wrap_error_t mysql_wrap_begin_transaction(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    return MYSQL_WRAP_ERR_NOT_CONNECTED;
}

mysql_wrap_error_t mysql_wrap_commit(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    return MYSQL_WRAP_ERR_NOT_CONNECTED;
}

mysql_wrap_error_t mysql_wrap_rollback(mysql_wrap_conn_t* conn) {
    if (!conn) return MYSQL_WRAP_ERR_NULL_PTR;
    return MYSQL_WRAP_ERR_NOT_CONNECTED;
}

char* mysql_wrap_escape_string(mysql_wrap_conn_t* conn, const char* str) {
    (void)conn; (void)str;
    return NULL;
}

#endif

mysql_wrap_conn_t* mysql_wrap_connect_simple(const char* host, unsigned int port, 
                                    const char* user, const char* password,
                                    const char* database, mysql_wrap_error_t* error) {
    mysql_config_t config;
    mysql_config_init(&config);
    
    if (host) mysql_config_set_host(&config, host);
    if (port > 0) mysql_config_set_port(&config, port);
    if (user) mysql_config_set_user(&config, user);
    if (password) mysql_config_set_password(&config, password);
    if (database) mysql_config_set_database(&config, database);
    
    return mysql_wrap_connect(&config, error);
}

bool mysql_wrap_is_connected(mysql_wrap_conn_t* conn) {
    return conn ? conn->connected : false;
}

uint64_t mysql_wrap_result_num_rows(mysql_wrap_result_t* result) {
    return result ? result->num_rows : 0;
}

unsigned int mysql_wrap_result_num_fields(mysql_wrap_result_t* result) {
    return result ? result->num_fields : 0;
}

const char* mysql_wrap_result_field_name(mysql_wrap_result_t* result, unsigned int index) {
#ifdef C_UTILS_HAS_MYSQL
    if (!result || !result->fields || index >= result->num_fields) return NULL;
    return result->fields[index].name;
#else
    (void)result; (void)index;
    return NULL;
#endif
}

int mysql_wrap_get_int_by_name(mysql_wrap_result_t* result, const char* name) {
    int index = mysql_wrap_get_field_index(result, name);
    return index >= 0 ? mysql_wrap_get_int(result, index) : 0;
}

int64_t mysql_wrap_get_int64_by_name(mysql_wrap_result_t* result, const char* name) {
    int index = mysql_wrap_get_field_index(result, name);
    return index >= 0 ? mysql_wrap_get_int64(result, index) : 0;
}

double mysql_wrap_get_double_by_name(mysql_wrap_result_t* result, const char* name) {
    int index = mysql_wrap_get_field_index(result, name);
    return index >= 0 ? mysql_wrap_get_double(result, index) : 0.0;
}

const char* mysql_wrap_get_string_by_name(mysql_wrap_result_t* result, const char* name) {
    int index = mysql_wrap_get_field_index(result, name);
    return index >= 0 ? mysql_wrap_get_string(result, index) : NULL;
}

mysql_wrap_pool_t* mysql_wrap_pool_create(const mysql_config_t* config, unsigned int min_size, unsigned int max_size) {
    if (!config) return NULL;
    
    mysql_wrap_pool_t* pool = (mysql_wrap_pool_t*)calloc(1, sizeof(mysql_wrap_pool_t));
    if (!pool) return NULL;
    
    memcpy(&pool->config, config, sizeof(mysql_config_t));
    pool->min_size = min_size;
    pool->max_size = max_size;
    pool->current_size = 0;
    
    pool->connections = (mysql_wrap_conn_t**)calloc(max_size, sizeof(mysql_wrap_conn_t*));
    pool->available = (bool*)calloc(max_size, sizeof(bool));
    
    if (!pool->connections || !pool->available) {
        free(pool->connections);
        free(pool->available);
        free(pool);
        return NULL;
    }
    
    mysql_wrap_error_t error;
    for (unsigned int i = 0; i < min_size; i++) {
        pool->connections[i] = mysql_wrap_connect(config, &error);
        if (pool->connections[i]) {
            pool->available[i] = true;
            pool->current_size++;
        }
    }
    
    return pool;
}

void mysql_wrap_pool_destroy(mysql_wrap_pool_t* pool) {
    if (!pool) return;
    
    for (unsigned int i = 0; i < pool->current_size; i++) {
        if (pool->connections[i]) {
            mysql_wrap_disconnect(pool->connections[i]);
        }
    }
    
    free(pool->connections);
    free(pool->available);
    free(pool);
}

mysql_wrap_conn_t* mysql_wrap_pool_get(mysql_wrap_pool_t* pool) {
    if (!pool) return NULL;
    
    for (unsigned int i = 0; i < pool->current_size; i++) {
        if (pool->available[i]) {
            pool->available[i] = false;
            return pool->connections[i];
        }
    }
    
    if (pool->current_size < pool->max_size) {
        mysql_wrap_error_t error;
        mysql_wrap_conn_t* conn = mysql_wrap_connect(&pool->config, &error);
        if (conn) {
            pool->connections[pool->current_size] = conn;
            pool->available[pool->current_size] = false;
            pool->current_size++;
            return conn;
        }
    }
    
    return NULL;
}

void mysql_wrap_pool_release(mysql_wrap_pool_t* pool, mysql_wrap_conn_t* conn) {
    if (!pool || !conn) return;
    
    for (unsigned int i = 0; i < pool->current_size; i++) {
        if (pool->connections[i] == conn) {
            pool->available[i] = true;
            return;
        }
    }
}

unsigned int mysql_wrap_pool_size(mysql_wrap_pool_t* pool) {
    return pool ? pool->current_size : 0;
}

unsigned int mysql_wrap_pool_available(mysql_wrap_pool_t* pool) {
    if (!pool) return 0;
    
    unsigned int count = 0;
    for (unsigned int i = 0; i < pool->current_size; i++) {
        if (pool->available[i]) count++;
    }
    return count;
}

mysql_wrap_error_t mysql_wrap_insert(mysql_wrap_conn_t* conn, const char* table, 
                                 const char** fields, const char** values, unsigned int count) {
    if (!conn || !table || !fields || !values || count == 0) {
        return MYSQL_WRAP_ERR_NULL_PTR;
    }
    
    size_t sql_size = strlen(table) + 100;
    for (unsigned int i = 0; i < count; i++) {
        sql_size += strlen(fields[i]) + strlen(values[i]) + 10;
    }
    
    char* sql = (char*)malloc(sql_size);
    if (!sql) return MYSQL_WRAP_ERR_MEMORY;
    
    char* ptr = sql;
    ptr += sprintf(ptr, "INSERT INTO %s (", table);
    
    for (unsigned int i = 0; i < count; i++) {
        ptr += sprintf(ptr, "%s", fields[i]);
        if (i < count - 1) ptr += sprintf(ptr, ", ");
    }
    
    ptr += sprintf(ptr, ") VALUES (");
    
    for (unsigned int i = 0; i < count; i++) {
        ptr += sprintf(ptr, "'%s'", values[i]);
        if (i < count - 1) ptr += sprintf(ptr, ", ");
    }
    
    ptr += sprintf(ptr, ")");
    
    int result = mysql_wrap_execute(conn, sql);
    free(sql);
    
    return result == 0 ? MYSQL_WRAP_OK : MYSQL_WRAP_ERR_QUERY;
}

mysql_wrap_error_t mysql_wrap_update(mysql_wrap_conn_t* conn, const char* table, 
                                 const char** fields, const char** values, unsigned int count, 
                                 const char* where) {
    if (!conn || !table || !fields || !values || count == 0) {
        return MYSQL_WRAP_ERR_NULL_PTR;
    }
    
    size_t sql_size = strlen(table) + 100;
    for (unsigned int i = 0; i < count; i++) {
        sql_size += strlen(fields[i]) + strlen(values[i]) + 10;
    }
    if (where) sql_size += strlen(where) + 10;
    
    char* sql = (char*)malloc(sql_size);
    if (!sql) return MYSQL_WRAP_ERR_MEMORY;
    
    char* ptr = sql;
    ptr += sprintf(ptr, "UPDATE %s SET ", table);
    
    for (unsigned int i = 0; i < count; i++) {
        ptr += sprintf(ptr, "%s = '%s'", fields[i], values[i]);
        if (i < count - 1) ptr += sprintf(ptr, ", ");
    }
    
    if (where) {
        ptr += sprintf(ptr, " WHERE %s", where);
    }
    
    int result = mysql_wrap_execute(conn, sql);
    free(sql);
    
    return result == 0 ? MYSQL_WRAP_OK : MYSQL_WRAP_ERR_QUERY;
}

mysql_wrap_error_t mysql_wrap_delete(mysql_wrap_conn_t* conn, const char* table, const char* where) {
    if (!conn || !table) return MYSQL_WRAP_ERR_NULL_PTR;
    
    char sql[1024];
    if (where) {
        snprintf(sql, sizeof(sql), "DELETE FROM %s WHERE %s", table, where);
    } else {
        snprintf(sql, sizeof(sql), "DELETE FROM %s", table);
    }
    
    int result = mysql_wrap_execute(conn, sql);
    return result == 0 ? MYSQL_WRAP_OK : MYSQL_WRAP_ERR_QUERY;
}

void mysql_wrap_free_string(char* str) {
    if (str) free(str);
}
