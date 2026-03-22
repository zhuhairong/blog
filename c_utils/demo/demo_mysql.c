#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_wrapper.h"

static void demo_connect(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    数据库连接演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    mysql_config_t config;
    mysql_config_init(&config);
    
    mysql_config_set_host(&config, "localhost");
    mysql_config_set_port(&config, 3306);
    mysql_config_set_user(&config, "root");
    mysql_config_set_password(&config, "password");
    mysql_config_set_database(&config, "test");
    mysql_config_set_charset(&config, "utf8mb4");
    
    printf("连接配置:\n");
    printf("  主机: %s:%u\n", config.host, config.port);
    printf("  用户: %s\n", config.user);
    printf("  数据库: %s\n", config.database);
    printf("  字符集: %s\n", config.charset);
    printf("  超时: %u秒\n", config.timeout);
    printf("  自动重连: %s\n", config.auto_reconnect ? "是" : "否");
    
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect(&config, &error);
    
    if (conn) {
        printf("\n连接成功! ✓\n");
        printf("连接状态: %s\n", mysql_wrap_is_connected(conn) ? "已连接" : "未连接");
        mysql_wrap_disconnect(conn);
        printf("已断开连接 ✓\n");
    } else {
        printf("\n连接失败: %s\n", mysql_wrap_error_string(error));
        printf("错误信息: %s\n", mysql_wrap_error(NULL));
    }
}

static void demo_simple_connect(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    简单连接演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect_simple("localhost", 3306, 
                                                "root", "password", 
                                                "test", &error);
    
    if (conn) {
        printf("简单连接成功! ✓\n");
        mysql_wrap_disconnect(conn);
    } else {
        printf("连接失败: %s\n", mysql_wrap_error_string(error));
    }
}

static void demo_query(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    查询操作演示                             \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("执行查询: SELECT VERSION()\n");
    
    mysql_wrap_result_t* result = mysql_wrap_query(conn, "SELECT VERSION() AS version, NOW() AS current_time");
    
    if (result) {
        printf("查询成功! ✓\n");
        printf("结果行数: %lu\n", (unsigned long)mysql_wrap_result_num_rows(result));
        printf("字段数: %u\n", mysql_wrap_result_num_fields(result));
        
        printf("\n字段名称:\n");
        for (unsigned int i = 0; i < mysql_wrap_result_num_fields(result); i++) {
            printf("  [%u] %s\n", i, mysql_wrap_result_field_name(result, i));
        }
        
        printf("\n查询结果:\n");
        while (mysql_wrap_result_next(result)) {
            const char* version = mysql_wrap_get_string_by_name(result, "version");
            const char* time = mysql_wrap_get_string_by_name(result, "current_time");
            printf("  版本: %s, 时间: %s\n", version ? version : "NULL", time ? time : "NULL");
        }
        
        mysql_wrap_result_free(result);
    } else {
        printf("查询失败: %s\n", mysql_wrap_error(conn));
    }
}

static void demo_create_table(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    创建表演示                                 \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  name VARCHAR(100) NOT NULL,"
        "  email VARCHAR(255),"
        "  age INT,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";
    
    printf("执行SQL: %s\n", sql);
    
    int result = mysql_wrap_execute(conn, sql);
    if (result == 0) {
        printf("创建表成功! ✓\n");
    } else {
        printf("创建表失败: %s\n", mysql_wrap_error(conn));
    }
}

static void demo_insert(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    插入数据演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    const char* fields[] = {"name", "email", "age"};
    const char* values[] = {"张三", "zhangsan@example.com", "25"};
    
    printf("插入数据: name='张三', email='zhangsan@example.com', age=25\n");
    
    mysql_wrap_error_t err = mysql_wrap_insert(conn, "users", fields, values, 3);
    if (err == MYSQL_WRAP_OK) {
        printf("插入成功! ✓\n");
        printf("最后插入ID: %lu\n", (unsigned long)mysql_wrap_last_insert_id(conn));
    } else {
        printf("插入失败: %s\n", mysql_wrap_error_string(err));
    }
}

static void demo_select(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    查询数据演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("执行查询: SELECT * FROM users\n");
    
    mysql_wrap_result_t* result = mysql_wrap_query(conn, "SELECT * FROM users ORDER BY id DESC LIMIT 10");
    if (result) {
        printf("查询成功! ✓\n");
        printf("结果行数: %lu\n\n", (unsigned long)mysql_wrap_result_num_rows(result));
        
        printf("%-5s %-15s %-25s %-5s %-20s\n", "ID", "姓名", "邮箱", "年龄", "创建时间");
        printf("───────────────────────────────────────────────────────────────\n");
        
        while (mysql_wrap_result_next(result)) {
            int id = mysql_wrap_get_int_by_name(result, "id");
            const char* name = mysql_wrap_get_string_by_name(result, "name");
            const char* email = mysql_wrap_get_string_by_name(result, "email");
            int age = mysql_wrap_get_int_by_name(result, "age");
            const char* created = mysql_wrap_get_string_by_name(result, "created_at");
            
            printf("%-5d %-15s %-25s %-5d %-20s\n", 
                   id, 
                   name ? name : "NULL", 
                   email ? email : "NULL", 
                   age,
                   created ? created : "NULL");
        }
        
        mysql_wrap_result_free(result);
    } else {
        printf("查询失败: %s\n", mysql_wrap_error(conn));
    }
}

static void demo_transaction(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    事务操作演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("开始事务...\n");
    mysql_wrap_error_t err = mysql_wrap_begin_transaction(conn);
    if (err != MYSQL_WRAP_OK) {
        printf("开始事务失败: %s\n", mysql_wrap_error_string(err));
        return;
    }
    printf("事务已开始 ✓\n");
    
    printf("执行插入操作...\n");
    const char* fields[] = {"name", "email", "age"};
    const char* values[] = {"李四", "lisi@example.com", "30"};
    err = mysql_wrap_insert(conn, "users", fields, values, 3);
    
    if (err != MYSQL_WRAP_OK) {
        printf("插入失败，执行回滚...\n");
        mysql_wrap_rollback(conn);
        printf("已回滚 ✓\n");
        return;
    }
    printf("插入成功 ✓\n");
    
    printf("提交事务...\n");
    err = mysql_wrap_commit(conn);
    if (err == MYSQL_WRAP_OK) {
        printf("事务已提交 ✓\n");
    } else {
        printf("提交失败: %s\n", mysql_wrap_error_string(err));
    }
}

static void demo_connection_pool(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    连接池演示                                  \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    mysql_config_t config;
    mysql_config_init(&config);
    mysql_config_set_host(&config, "localhost");
    mysql_config_set_user(&config, "root");
    mysql_config_set_password(&config, "password");
    mysql_config_set_database(&config, "test");
    
    printf("创建连接池 (最小: 2, 最大: 5)...\n");
    mysql_wrap_pool_t* pool = mysql_wrap_pool_create(&config, 2, 5);
    
    if (pool) {
        printf("连接池创建成功! ✓\n");
        printf("当前连接数: %u\n", mysql_wrap_pool_size(pool));
        printf("可用连接数: %u\n", mysql_wrap_pool_available(pool));
        
        printf("\n从连接池获取连接...\n");
        mysql_wrap_conn_t* conn = mysql_wrap_pool_get(pool);
        if (conn) {
            printf("获取连接成功! ✓\n");
            printf("当前连接数: %u\n", mysql_wrap_pool_size(pool));
            printf("可用连接数: %u\n", mysql_wrap_pool_available(pool));
            
            printf("\n执行查询...\n");
            mysql_wrap_result_t* result = mysql_wrap_query(conn, "SELECT 1 AS test");
            if (result) {
                printf("查询成功! ✓\n");
                mysql_wrap_result_free(result);
            }
            
            printf("\n归还连接到连接池...\n");
            mysql_wrap_pool_release(pool, conn);
            printf("已归还 ✓\n");
            printf("当前连接数: %u\n", mysql_wrap_pool_size(pool));
            printf("可用连接数: %u\n", mysql_wrap_pool_available(pool));
        }
        
        printf("\n销毁连接池...\n");
        mysql_wrap_pool_destroy(pool);
        printf("连接池已销毁 ✓\n");
    } else {
        printf("创建连接池失败\n");
    }
}

static void demo_escape_string(mysql_wrap_conn_t* conn) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    字符串转义演示                              \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    const char* input = "It's a test with 'quotes' and \"double quotes\"";
    printf("原始字符串: %s\n", input);
    
    char* escaped = mysql_wrap_escape_string(conn, input);
    if (escaped) {
        printf("转义后字符串: %s\n", escaped);
        printf("转义成功! ✓\n");
        mysql_wrap_free_string(escaped);
    } else {
        printf("转义失败\n");
    }
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    MySQL封装库演示程序                         \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    demo_connect();
    demo_simple_connect();
    
    mysql_wrap_error_t error;
    mysql_wrap_conn_t* conn = mysql_wrap_connect_simple("localhost", 3306, 
                                                "root", "password", 
                                                "test", &error);
    
    if (conn) {
        demo_query(conn);
        demo_create_table(conn);
        demo_insert(conn);
        demo_select(conn);
        demo_transaction(conn);
        demo_escape_string(conn);
        mysql_wrap_disconnect(conn);
    } else {
        printf("\n注意: 无法连接到MySQL服务器，跳过需要连接的演示。\n");
        printf("请确保MySQL服务器正在运行，并且连接参数正确。\n");
    }
    
    demo_connection_pool();
    
    printf("\n演示完成!\n\n");
    return 0;
}
