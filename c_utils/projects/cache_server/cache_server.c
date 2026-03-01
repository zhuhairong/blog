#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#include "lru_cache.h"
#include "threadpool.h"
#include "net.h"
#include "timer.h"
#include "terminal.h"
#include "argparse.h"

#define DEFAULT_PORT "6379"
#define DEFAULT_CAPACITY 10000
#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN (1024 * 1024)
#define BUFFER_SIZE 4096
#define MAX_EXPIRE_ENTRIES 10000

typedef struct {
    char key[MAX_KEY_LEN];
    uint64_t expire_time;
} expire_entry_t;

typedef struct {
    lru_cache_t *cache;
    expire_entry_t *expire_list;
    size_t expire_count;
    size_t expire_capacity;
    threadpool_t *pool;
    socket_t server_fd;
    volatile bool running;
    pthread_mutex_t lock;
    
    size_t total_connections;
    size_t active_connections;
    size_t commands_processed;
    size_t expired_keys;
    time_t start_time;
} cache_server_t;

static cache_server_t g_server;

typedef struct {
    socket_t client_fd;
    char client_ip[INET6_ADDRSTRLEN];
} client_context_t;

static uint64_t get_current_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static expire_entry_t* find_expire_entry(const char *key) {
    for (size_t i = 0; i < g_server.expire_count; i++) {
        if (strcmp(g_server.expire_list[i].key, key) == 0) {
            return &g_server.expire_list[i];
        }
    }
    return NULL;
}

static void remove_expire_entry(const char *key) {
    for (size_t i = 0; i < g_server.expire_count; i++) {
        if (strcmp(g_server.expire_list[i].key, key) == 0) {
            memmove(&g_server.expire_list[i], 
                    &g_server.expire_list[g_server.expire_count - 1],
                    sizeof(expire_entry_t));
            g_server.expire_count--;
            return;
        }
    }
}

static void add_expire_entry(const char *key, uint64_t expire_time) {
    expire_entry_t *existing = find_expire_entry(key);
    if (existing) {
        existing->expire_time = expire_time;
        return;
    }
    
    if (g_server.expire_count >= g_server.expire_capacity) {
        size_t new_capacity = g_server.expire_capacity * 2;
        expire_entry_t *new_list = realloc(g_server.expire_list, 
                                           new_capacity * sizeof(expire_entry_t));
        if (!new_list) return;
        g_server.expire_list = new_list;
        g_server.expire_capacity = new_capacity;
    }
    
    strncpy(g_server.expire_list[g_server.expire_count].key, key, MAX_KEY_LEN - 1);
    g_server.expire_list[g_server.expire_count].key[MAX_KEY_LEN - 1] = '\0';
    g_server.expire_list[g_server.expire_count].expire_time = expire_time;
    g_server.expire_count++;
}

static void check_expired_keys(void *data) {
    (void)data;
    
    pthread_mutex_lock(&g_server.lock);
    
    uint64_t now = get_current_time_ms();
    size_t count = 0;
    
    for (size_t i = 0; i < g_server.expire_count; ) {
        if (g_server.expire_list[i].expire_time > 0 && 
            g_server.expire_list[i].expire_time <= now) {
            lru_cache_remove(g_server.cache, g_server.expire_list[i].key);
            
            memmove(&g_server.expire_list[i], 
                    &g_server.expire_list[g_server.expire_count - 1],
                    sizeof(expire_entry_t));
            g_server.expire_count--;
            count++;
        } else {
            i++;
        }
    }
    
    g_server.expired_keys += count;
    
    pthread_mutex_unlock(&g_server.lock);
}

static void send_response(socket_t fd, const char *response) {
    size_t len = strlen(response);
    net_send(fd, response, len);
}

static void handle_set(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    char *value = strtok(NULL, "\r\n");
    char *expire_str = strtok(NULL, " \t\r\n");
    
    if (!key || !value) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'set' command\r\n");
        return;
    }
    
    size_t value_len = strlen(value);
    if (value_len > MAX_VALUE_LEN) {
        send_response(ctx->client_fd, "-ERR value too large\r\n");
        return;
    }
    
    char *value_copy = strdup(value);
    if (!value_copy) {
        send_response(ctx->client_fd, "-ERR out of memory\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    lru_cache_put(g_server.cache, key, value_copy);
    
    if (expire_str) {
        int expire_seconds = atoi(expire_str);
        if (expire_seconds > 0) {
            uint64_t expire_time = get_current_time_ms() + (uint64_t)expire_seconds * 1000;
            add_expire_entry(key, expire_time);
        }
    } else {
        remove_expire_entry(key);
    }
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    send_response(ctx->client_fd, "+OK\r\n");
}

static void handle_get(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'get' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    uint64_t now = get_current_time_ms();
    expire_entry_t *entry = find_expire_entry(key);
    if (entry && entry->expire_time > 0 && entry->expire_time <= now) {
        lru_cache_remove(g_server.cache, key);
        remove_expire_entry(key);
        g_server.expired_keys++;
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, "$-1\r\n");
        return;
    }
    
    char *value = lru_cache_get(g_server.cache, key);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    if (value) {
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(value), value);
        send_response(ctx->client_fd, response);
    } else {
        send_response(ctx->client_fd, "$-1\r\n");
    }
}

static void handle_del(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'del' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    bool existed = lru_cache_contains(g_server.cache, key);
    if (existed) {
        lru_cache_remove(g_server.cache, key);
        remove_expire_entry(key);
    }
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%d\r\n", existed ? 1 : 0);
    send_response(ctx->client_fd, response);
}

static void handle_exists(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'exists' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    uint64_t now = get_current_time_ms();
    expire_entry_t *entry = find_expire_entry(key);
    if (entry && entry->expire_time > 0 && entry->expire_time <= now) {
        lru_cache_remove(g_server.cache, key);
        remove_expire_entry(key);
        g_server.expired_keys++;
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":0\r\n");
        return;
    }
    
    bool exists = lru_cache_contains(g_server.cache, key);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%d\r\n", exists ? 1 : 0);
    send_response(ctx->client_fd, response);
}

static void handle_expire(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    char *seconds_str = strtok(NULL, " \t\r\n");
    
    if (!key || !seconds_str) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'expire' command\r\n");
        return;
    }
    
    int seconds = atoi(seconds_str);
    if (seconds <= 0) {
        send_response(ctx->client_fd, "-ERR invalid expire time\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    bool exists = lru_cache_contains(g_server.cache, key);
    if (!exists) {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":0\r\n");
        return;
    }
    
    uint64_t expire_time = get_current_time_ms() + (uint64_t)seconds * 1000;
    add_expire_entry(key, expire_time);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    send_response(ctx->client_fd, ":1\r\n");
}

static void handle_ttl(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'ttl' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    bool exists = lru_cache_contains(g_server.cache, key);
    if (!exists) {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":-2\r\n");
        return;
    }
    
    expire_entry_t *entry = find_expire_entry(key);
    if (!entry || entry->expire_time == 0) {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":-1\r\n");
        return;
    }
    
    uint64_t now = get_current_time_ms();
    int64_t ttl_ms = (int64_t)(entry->expire_time - now);
    int ttl_seconds = (int)(ttl_ms / 1000);
    
    if (ttl_seconds <= 0) {
        lru_cache_remove(g_server.cache, key);
        remove_expire_entry(key);
        g_server.expired_keys++;
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":-2\r\n");
        return;
    }
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%d\r\n", ttl_seconds);
    send_response(ctx->client_fd, response);
}

static void handle_incr(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'incr' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    char *value = lru_cache_get(g_server.cache, key);
    
    if (!value) {
        char *new_value = strdup("1");
        if (!new_value) {
            pthread_mutex_unlock(&g_server.lock);
            send_response(ctx->client_fd, "-ERR out of memory\r\n");
            return;
        }
        lru_cache_put(g_server.cache, key, new_value);
        g_server.commands_processed++;
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":1\r\n");
        return;
    }
    
    char *endptr;
    long long num = strtoll(value, &endptr, 10);
    if (*endptr != '\0') {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, "-ERR value is not an integer or out of range\r\n");
        return;
    }
    
    num++;
    char new_value[64];
    snprintf(new_value, sizeof(new_value), "%lld", num);
    
    char *value_copy = strdup(new_value);
    if (!value_copy) {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, "-ERR out of memory\r\n");
        return;
    }
    
    lru_cache_put(g_server.cache, key, value_copy);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%lld\r\n", num);
    send_response(ctx->client_fd, response);
}

static void handle_decr(client_context_t *ctx, char *args) {
    char *key = strtok(args, " \t\r\n");
    
    if (!key) {
        send_response(ctx->client_fd, "-ERR wrong number of arguments for 'decr' command\r\n");
        return;
    }
    
    pthread_mutex_lock(&g_server.lock);
    
    char *value = lru_cache_get(g_server.cache, key);
    
    if (!value) {
        char *new_value = strdup("-1");
        if (!new_value) {
            pthread_mutex_unlock(&g_server.lock);
            send_response(ctx->client_fd, "-ERR out of memory\r\n");
            return;
        }
        lru_cache_put(g_server.cache, key, new_value);
        g_server.commands_processed++;
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, ":-1\r\n");
        return;
    }
    
    char *endptr;
    long long num = strtoll(value, &endptr, 10);
    if (*endptr != '\0') {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, "-ERR value is not an integer or out of range\r\n");
        return;
    }
    
    num--;
    char new_value[64];
    snprintf(new_value, sizeof(new_value), "%lld", num);
    
    char *value_copy = strdup(new_value);
    if (!value_copy) {
        pthread_mutex_unlock(&g_server.lock);
        send_response(ctx->client_fd, "-ERR out of memory\r\n");
        return;
    }
    
    lru_cache_put(g_server.cache, key, value_copy);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%lld\r\n", num);
    send_response(ctx->client_fd, response);
}

static void handle_flushall(client_context_t *ctx) {
    pthread_mutex_lock(&g_server.lock);
    
    lru_cache_clear(g_server.cache);
    g_server.expire_count = 0;
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    send_response(ctx->client_fd, "+OK\r\n");
}

static void handle_dbsize(client_context_t *ctx) {
    pthread_mutex_lock(&g_server.lock);
    
    size_t size = lru_cache_size(g_server.cache);
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char response[64];
    snprintf(response, sizeof(response), ":%zu\r\n", size);
    send_response(ctx->client_fd, response);
}

static void handle_info(client_context_t *ctx) {
    pthread_mutex_lock(&g_server.lock);
    
    lru_cache_stats_t stats;
    lru_cache_get_stats(g_server.cache, &stats);
    
    time_t now = time(NULL);
    time_t uptime = now - g_server.start_time;
    
    char response[2048];
    snprintf(response, sizeof(response),
        "# Server\r\n"
        "uptime_in_seconds:%ld\r\n"
        "connected_clients:%zu\r\n"
        "\r\n"
        "# Stats\r\n"
        "total_connections_received:%zu\r\n"
        "total_commands_processed:%zu\r\n"
        "expired_keys:%zu\r\n"
        "\r\n"
        "# Memory\r\n"
        "cache_size:%zu\r\n"
        "cache_capacity:%zu\r\n"
        "cache_hits:%zu\r\n"
        "cache_misses:%zu\r\n"
        "cache_evictions:%zu\r\n"
        "hit_rate:%.2f%%\r\n"
        "\r\n",
        uptime, g_server.active_connections,
        g_server.total_connections, g_server.commands_processed,
        g_server.expired_keys,
        stats.current_size, stats.capacity,
        stats.hits, stats.misses, stats.evictions,
        stats.hit_rate * 100.0
    );
    
    g_server.commands_processed++;
    
    pthread_mutex_unlock(&g_server.lock);
    
    char full_response[2200];
    snprintf(full_response, sizeof(full_response), "$%zu\r\n%s", strlen(response), response);
    send_response(ctx->client_fd, full_response);
}

static void handle_ping(client_context_t *ctx) {
    send_response(ctx->client_fd, "+PONG\r\n");
}

static void handle_quit(client_context_t *ctx) {
    send_response(ctx->client_fd, "+OK\r\n");
}

static void process_command(client_context_t *ctx, char *buffer) {
    char *line = buffer;
    
    while (*line == ' ' || *line == '\t') line++;
    
    char *cmd_end = line;
    while (*cmd_end && *cmd_end != ' ' && *cmd_end != '\t' && *cmd_end != '\r' && *cmd_end != '\n') {
        cmd_end++;
    }
    
    char cmd[64] = {0};
    size_t cmd_len = cmd_end - line;
    if (cmd_len >= sizeof(cmd)) cmd_len = sizeof(cmd) - 1;
    strncpy(cmd, line, cmd_len);
    
    char *args = cmd_end;
    while (*args == ' ' || *args == '\t') args++;
    
    for (size_t i = 0; cmd[i]; i++) {
        cmd[i] = tolower((unsigned char)cmd[i]);
    }
    
    if (strcmp(cmd, "set") == 0) {
        handle_set(ctx, args);
    } else if (strcmp(cmd, "get") == 0) {
        handle_get(ctx, args);
    } else if (strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0) {
        handle_del(ctx, args);
    } else if (strcmp(cmd, "exists") == 0) {
        handle_exists(ctx, args);
    } else if (strcmp(cmd, "expire") == 0) {
        handle_expire(ctx, args);
    } else if (strcmp(cmd, "ttl") == 0) {
        handle_ttl(ctx, args);
    } else if (strcmp(cmd, "incr") == 0) {
        handle_incr(ctx, args);
    } else if (strcmp(cmd, "decr") == 0) {
        handle_decr(ctx, args);
    } else if (strcmp(cmd, "flushall") == 0) {
        handle_flushall(ctx);
    } else if (strcmp(cmd, "dbsize") == 0) {
        handle_dbsize(ctx);
    } else if (strcmp(cmd, "info") == 0) {
        handle_info(ctx);
    } else if (strcmp(cmd, "ping") == 0) {
        handle_ping(ctx);
    } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
        handle_quit(ctx);
    } else {
        char response[128];
        snprintf(response, sizeof(response), "-ERR unknown command '%s'\r\n", cmd);
        send_response(ctx->client_fd, response);
    }
}

static void handle_client(void *arg) {
    client_context_t *ctx = (client_context_t*)arg;
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE * 2];
    size_t line_len = 0;
    
    printf("Client connected from %s\n", ctx->client_ip);
    
    pthread_mutex_lock(&g_server.lock);
    g_server.active_connections++;
    pthread_mutex_unlock(&g_server.lock);
    
    while (g_server.running) {
        int n = net_recv(ctx->client_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            break;
        }
        
        buffer[n] = '\0';
        
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                line_buffer[line_len] = '\0';
                
                if (line_len > 0 && line_buffer[line_len - 1] == '\r') {
                    line_buffer[line_len - 1] = '\0';
                }
                
                if (line_len > 0) {
                    process_command(ctx, line_buffer);
                    
                    if (strncasecmp(line_buffer, "quit", 4) == 0 || 
                        strncasecmp(line_buffer, "exit", 4) == 0) {
                        goto cleanup;
                    }
                }
                
                line_len = 0;
            } else {
                if (line_len < sizeof(line_buffer) - 1) {
                    line_buffer[line_len++] = buffer[i];
                }
            }
        }
    }
    
cleanup:
    pthread_mutex_lock(&g_server.lock);
    g_server.active_connections--;
    pthread_mutex_unlock(&g_server.lock);
    
    net_close(ctx->client_fd);
    printf("Client disconnected from %s\n", ctx->client_ip);
    free(ctx);
}

static void signal_handler(int sig) {
    (void)sig;
    g_server.running = false;
    printf("\nShutting down server...\n");
}

static void print_banner(void) {
    term_printf(TERM_ANSI_CYAN,
        "╔════════════════════════════════════════════════════════════╗\n"
        "║                    缓存服务器 v1.0                          ║\n"
        "╠════════════════════════════════════════════════════════════╣\n"
        "║  基于 c_utils 库构建                                       ║\n"
        "║  使用 LRU 缓存淘汰策略                                      ║\n"
        "╚════════════════════════════════════════════════════════════╝\n"
        TERM_ANSI_RESET);
}

static void print_help(const char *prog) {
    print_banner();
    printf("\n用法: %s [选项]\n\n", prog);
    printf("选项:\n");
    printf("  -p, --port <port>      监听端口 (默认: %s)\n", DEFAULT_PORT);
    printf("  -c, --capacity <num>   缓存容量 (默认: %d)\n", DEFAULT_CAPACITY);
    printf("  -t, --threads <num>    工作线程数 (默认: CPU核心数)\n");
    printf("  -h, --help             显示帮助信息\n");
    printf("\n支持的命令:\n");
    printf("  SET <key> <value> [expire]  设置键值对\n");
    printf("  GET <key>                   获取值\n");
    printf("  DEL <key>                   删除键\n");
    printf("  EXISTS <key>                检查键是否存在\n");
    printf("  EXPIRE <key> <seconds>      设置过期时间\n");
    printf("  TTL <key>                   获取剩余过期时间\n");
    printf("  INCR <key>                  递增\n");
    printf("  DECR <key>                  递减\n");
    printf("  FLUSHALL                    清空所有数据\n");
    printf("  DBSIZE                      获取键数量\n");
    printf("  INFO                        获取服务器信息\n");
    printf("  PING                        测试连接\n");
    printf("  QUIT                        关闭连接\n");
}

int main(int argc, char *argv[]) {
    const char *port = DEFAULT_PORT;
    size_t capacity = DEFAULT_CAPACITY;
    int num_threads = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                port = argv[++i];
            }
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--capacity") == 0) {
            if (i + 1 < argc) {
                capacity = (size_t)atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
            if (i + 1 < argc) {
                num_threads = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
    }
    
    if (capacity == 0) {
        capacity = DEFAULT_CAPACITY;
    }
    
    memset(&g_server, 0, sizeof(g_server));
    g_server.running = true;
    g_server.start_time = time(NULL);
    pthread_mutex_init(&g_server.lock, NULL);
    
    g_server.expire_capacity = MAX_EXPIRE_ENTRIES;
    g_server.expire_list = malloc(g_server.expire_capacity * sizeof(expire_entry_t));
    if (!g_server.expire_list) {
        fprintf(stderr, "Failed to create expire list\n");
        return 1;
    }
    
    g_server.cache = lru_cache_create(capacity);
    if (!g_server.cache) {
        fprintf(stderr, "Failed to create cache\n");
        free(g_server.expire_list);
        return 1;
    }
    
    g_server.pool = threadpool_create(num_threads);
    if (!g_server.pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        lru_cache_free(g_server.cache);
        free(g_server.expire_list);
        return 1;
    }
    
    if (!net_init()) {
        fprintf(stderr, "Failed to initialize network\n");
        lru_cache_free(g_server.cache);
        free(g_server.expire_list);
        threadpool_destroy(g_server.pool);
        return 1;
    }
    
    g_server.server_fd = net_listen(port);
    if (g_server.server_fd == INVALID_SOCKET) {
        fprintf(stderr, "Failed to listen on port %s\n", port);
        net_cleanup();
        lru_cache_free(g_server.cache);
        free(g_server.expire_list);
        threadpool_destroy(g_server.pool);
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    print_banner();
    printf("\n服务器启动:\n");
    printf("  端口: %s\n", port);
    printf("  缓存容量: %zu\n", capacity);
    printf("  工作线程: %d\n", threadpool_get_thread_count(g_server.pool));
    printf("\n等待客户端连接...\n");
    printf("使用 telnet localhost %s 或 nc localhost %s 连接\n\n", port, port);
    
    cutils_timer_t expire_timer;
    timer_set(&expire_timer, 1000, check_expired_keys, NULL);
    
    while (g_server.running) {
        timer_update(&expire_timer);
        
        socket_t client_fd = net_accept(g_server.server_fd);
        
        if (client_fd == INVALID_SOCKET) {
            if (g_server.running) {
                usleep(10000);
            }
            continue;
        }
        
        client_context_t *ctx = malloc(sizeof(client_context_t));
        if (!ctx) {
            net_close(client_fd);
            continue;
        }
        
        ctx->client_fd = client_fd;
        strncpy(ctx->client_ip, "unknown", sizeof(ctx->client_ip) - 1);
        
        pthread_mutex_lock(&g_server.lock);
        g_server.total_connections++;
        pthread_mutex_unlock(&g_server.lock);
        
        threadpool_add_task(g_server.pool, handle_client, ctx);
    }
    
    printf("正在关闭服务器...\n");
    
    net_close(g_server.server_fd);
    threadpool_wait_all(g_server.pool, 5000);
    threadpool_destroy(g_server.pool);
    
    lru_cache_free(g_server.cache);
    free(g_server.expire_list);
    
    pthread_mutex_destroy(&g_server.lock);
    net_cleanup();
    
    printf("服务器已关闭\n");
    return 0;
}
