#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

#include "list.h"
#include "threadpool.h"
#include "net.h"
#include "terminal.h"
#include "json.h"
#include "fs_utils.h"

#define DEFAULT_PORT "5672"
#define MAX_QUEUE_NAME 128
#define MAX_MESSAGE_SIZE (64 * 1024)
#define MAX_QUEUES 256
#define MAX_CONSUMERS_PER_QUEUE 64
#define DEFAULT_QUEUE_CAPACITY 10000
#define PERSIST_FILE "mq_data.json"

typedef enum {
    MSG_STATUS_PENDING = 0,
    MSG_STATUS_DELIVERED = 1,
    MSG_STATUS_ACKNOWLEDGED = 2,
    MSG_STATUS_REJECTED = 3
} message_status_t;

typedef struct {
    uint64_t id;
    char queue_name[MAX_QUEUE_NAME];
    char *body;
    size_t body_len;
    uint64_t timestamp;
    uint64_t expire_time;
    uint8_t priority;
    message_status_t status;
    int delivery_count;
    char content_type[64];
    char correlation_id[64];
    char reply_to[MAX_QUEUE_NAME];
} message_t;

typedef struct {
    char name[MAX_QUEUE_NAME];
    list_t *messages;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    
    size_t capacity;
    bool durable;
    bool auto_delete;
    uint32_t message_ttl;
    uint32_t max_length;
    
    size_t message_count;
    size_t consumer_count;
    uint64_t total_published;
    uint64_t total_consumed;
    uint64_t total_acked;
    uint64_t total_nacked;
} queue_t;

typedef struct {
    char consumer_id[64];
    char queue_name[MAX_QUEUE_NAME];
    socket_t client_fd;
    bool active;
    bool exclusive;
    uint32_t prefetch_count;
    uint32_t unacked_count;
} consumer_t;

typedef struct {
    queue_t *queues[MAX_QUEUES];
    size_t queue_count;
    consumer_t *consumers[MAX_QUEUES * MAX_CONSUMERS_PER_QUEUE];
    size_t consumer_count;
    pthread_mutex_t global_lock;
    threadpool_t *pool;
    socket_t server_fd;
    volatile bool running;
    
    uint64_t next_message_id;
    uint64_t next_consumer_id;
    time_t start_time;
    
    size_t total_connections;
    size_t active_connections;
} mq_server_t;

static mq_server_t g_mq;

typedef struct {
    socket_t client_fd;
    char client_ip[INET6_ADDRSTRLEN];
} client_context_t;

static uint64_t get_current_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void free_message(void *data) {
    if (data) {
        message_t *msg = (message_t*)data;
        if (msg->body) free(msg->body);
        free(msg);
    }
}

static queue_t* find_queue(const char *name) {
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        if (strcmp(g_mq.queues[i]->name, name) == 0) {
            return g_mq.queues[i];
        }
    }
    return NULL;
}

static consumer_t* find_consumer(const char *consumer_id) {
    for (size_t i = 0; i < g_mq.consumer_count; i++) {
        if (strcmp(g_mq.consumers[i]->consumer_id, consumer_id) == 0) {
            return g_mq.consumers[i];
        }
    }
    return NULL;
}

static queue_t* create_queue(const char *name, bool durable, bool auto_delete, 
                             uint32_t message_ttl, uint32_t max_length) {
    if (g_mq.queue_count >= MAX_QUEUES) return NULL;
    if (find_queue(name) != NULL) return NULL;
    
    queue_t *q = calloc(1, sizeof(queue_t));
    if (!q) return NULL;
    
    strncpy(q->name, name, MAX_QUEUE_NAME - 1);
    q->messages = list_create();
    if (!q->messages) {
        free(q);
        return NULL;
    }
    
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
    
    q->capacity = DEFAULT_QUEUE_CAPACITY;
    q->durable = durable;
    q->auto_delete = auto_delete;
    q->message_ttl = message_ttl;
    q->max_length = max_length;
    
    g_mq.queues[g_mq.queue_count++] = q;
    return q;
}

static void delete_queue(queue_t *q) {
    if (!q) return;
    
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        if (g_mq.queues[i] == q) {
            memmove(&g_mq.queues[i], &g_mq.queues[i + 1], 
                    (g_mq.queue_count - i - 1) * sizeof(queue_t*));
            g_mq.queue_count--;
            break;
        }
    }
    
    list_free_with_data(q->messages, free_message);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->cond);
    free(q);
}

static int compare_message_priority(const void *a, const void *b) {
    const message_t *ma = (const message_t*)a;
    const message_t *mb = (const message_t*)b;
    return mb->priority - ma->priority;
}

static message_t* create_message(const char *queue_name, const char *body, size_t body_len,
                                 uint8_t priority, uint32_t ttl, const char *content_type,
                                 const char *correlation_id, const char *reply_to) {
    message_t *msg = calloc(1, sizeof(message_t));
    if (!msg) return NULL;
    
    msg->id = __sync_fetch_and_add(&g_mq.next_message_id, 1);
    strncpy(msg->queue_name, queue_name, MAX_QUEUE_NAME - 1);
    
    msg->body = malloc(body_len + 1);
    if (!msg->body) {
        free(msg);
        return NULL;
    }
    memcpy(msg->body, body, body_len);
    msg->body[body_len] = '\0';
    msg->body_len = body_len;
    
    msg->timestamp = get_current_time_ms();
    msg->expire_time = ttl > 0 ? msg->timestamp + (uint64_t)ttl * 1000 : 0;
    msg->priority = priority;
    msg->status = MSG_STATUS_PENDING;
    msg->delivery_count = 0;
    
    if (content_type) strncpy(msg->content_type, content_type, 63);
    if (correlation_id) strncpy(msg->correlation_id, correlation_id, 63);
    if (reply_to) strncpy(msg->reply_to, reply_to, MAX_QUEUE_NAME - 1);
    
    return msg;
}

static bool is_message_expired(const message_t *msg) {
    if (msg->expire_time == 0) return false;
    return get_current_time_ms() >= msg->expire_time;
}

static void purge_expired_messages(queue_t *q) {
    list_node_t *node = q->messages->head;
    list_node_t *next;
    
    while (node) {
        next = node->next;
        message_t *msg = (message_t*)node->data;
        if (is_message_expired(msg)) {
            list_remove_node(q->messages, node);
            free_message(msg);
            q->message_count--;
        }
        node = next;
    }
}

static void send_response(socket_t fd, const char *response) {
    net_send(fd, response, strlen(response));
}

static void send_json_response(socket_t fd, const char *status, const char *message, json_value_t *data) {
    char *response = NULL;
    
    if (data) {
        char *data_str = json_print(data);
        if (message && message[0]) {
            asprintf(&response, "{\"status\":\"%s\",\"message\":\"%s\",\"data\":%s}\n", 
                     status, message, data_str);
        } else {
            asprintf(&response, "{\"status\":\"%s\",\"data\":%s}\n", status, data_str);
        }
        free(data_str);
    } else {
        if (message && message[0]) {
            asprintf(&response, "{\"status\":\"%s\",\"message\":\"%s\"}\n", status, message);
        } else {
            asprintf(&response, "{\"status\":\"%s\"}\n", status);
        }
    }
    
    if (response) {
        send_response(fd, response);
        free(response);
    }
}

static void handle_declare_queue(client_context_t *ctx, json_value_t *params) {
    json_value_t *name_val = json_object_get(params, "name");
    if (!name_val || name_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue name", NULL);
        return;
    }
    
    const char *name = json_as_string(name_val);
    
    bool durable = false;
    bool auto_delete = false;
    uint32_t message_ttl = 0;
    uint32_t max_length = 0;
    
    json_value_t *durable_val = json_object_get(params, "durable");
    if (durable_val && durable_val->type == JSON_BOOL) {
        durable = durable_val->u.boolean;
    }
    
    json_value_t *auto_delete_val = json_object_get(params, "auto_delete");
    if (auto_delete_val && auto_delete_val->type == JSON_BOOL) {
        auto_delete = auto_delete_val->u.boolean;
    }
    
    json_value_t *ttl_val = json_object_get(params, "message_ttl");
    if (ttl_val && ttl_val->type == JSON_NUMBER) {
        message_ttl = (uint32_t)ttl_val->u.number;
    }
    
    json_value_t *max_len_val = json_object_get(params, "max_length");
    if (max_len_val && max_len_val->type == JSON_NUMBER) {
        max_length = (uint32_t)max_len_val->u.number;
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(name);
    if (!q) {
        q = create_queue(name, durable, auto_delete, message_ttl, max_length);
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    if (q) {
        send_json_response(ctx->client_fd, "ok", "queue declared", NULL);
    } else {
        send_json_response(ctx->client_fd, "error", "failed to declare queue", NULL);
    }
}

static void handle_delete_queue(client_context_t *ctx, json_value_t *params) {
    json_value_t *name_val = json_object_get(params, "name");
    if (!name_val || name_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue name", NULL);
        return;
    }
    
    const char *name = json_as_string(name_val);
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(name);
    if (q) {
        delete_queue(q);
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "ok", "queue deleted", NULL);
    } else {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "queue not found", NULL);
    }
}

static void handle_publish(client_context_t *ctx, json_value_t *params) {
    json_value_t *queue_val = json_object_get(params, "queue");
    json_value_t *body_val = json_object_get(params, "body");
    
    if (!queue_val || queue_val->type != JSON_STRING ||
        !body_val || body_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue or body", NULL);
        return;
    }
    
    const char *queue_name = json_as_string(queue_val);
    const char *body = json_as_string(body_val);
    size_t body_len = strlen(body);
    
    if (body_len > MAX_MESSAGE_SIZE) {
        send_json_response(ctx->client_fd, "error", "message too large", NULL);
        return;
    }
    
    uint8_t priority = 0;
    uint32_t ttl = 0;
    const char *content_type = NULL;
    const char *correlation_id = NULL;
    const char *reply_to = NULL;
    
    json_value_t *priority_val = json_object_get(params, "priority");
    if (priority_val && priority_val->type == JSON_NUMBER) {
        priority = (uint8_t)priority_val->u.number;
    }
    
    json_value_t *ttl_val = json_object_get(params, "ttl");
    if (ttl_val && ttl_val->type == JSON_NUMBER) {
        ttl = (uint32_t)ttl_val->u.number;
    }
    
    json_value_t *ct_val = json_object_get(params, "content_type");
    if (ct_val && ct_val->type == JSON_STRING) {
        content_type = json_as_string(ct_val);
    }
    
    json_value_t *cid_val = json_object_get(params, "correlation_id");
    if (cid_val && cid_val->type == JSON_STRING) {
        correlation_id = json_as_string(cid_val);
    }
    
    json_value_t *reply_val = json_object_get(params, "reply_to");
    if (reply_val && reply_val->type == JSON_STRING) {
        reply_to = json_as_string(reply_val);
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(queue_name);
    if (!q) {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "queue not found", NULL);
        return;
    }
    
    pthread_mutex_lock(&q->lock);
    pthread_mutex_unlock(&g_mq.global_lock);
    
    purge_expired_messages(q);
    
    if (q->max_length > 0 && q->message_count >= q->max_length) {
        pthread_mutex_unlock(&q->lock);
        send_json_response(ctx->client_fd, "error", "queue is full", NULL);
        return;
    }
    
    message_t *msg = create_message(queue_name, body, body_len, priority, ttl,
                                    content_type, correlation_id, reply_to);
    if (!msg) {
        pthread_mutex_unlock(&q->lock);
        send_json_response(ctx->client_fd, "error", "failed to create message", NULL);
        return;
    }
    
    list_push_back(q->messages, msg);
    q->message_count++;
    q->total_published++;
    
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_OBJECT;
    result->u.object.keys = malloc(sizeof(char*));
    result->u.object.values = malloc(sizeof(json_value_t*));
    result->u.object.count = 1;
    
    result->u.object.keys[0] = strdup("message_id");
    result->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[0]->type = JSON_NUMBER;
    result->u.object.values[0]->u.number = (double)msg->id;
    
    send_json_response(ctx->client_fd, "ok", "message published", result);
    json_free(result);
}

static void handle_consume(client_context_t *ctx, json_value_t *params) {
    json_value_t *queue_val = json_object_get(params, "queue");
    if (!queue_val || queue_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue name", NULL);
        return;
    }
    
    const char *queue_name = json_as_string(queue_val);
    
    uint32_t prefetch = 1;
    bool exclusive = false;
    
    json_value_t *prefetch_val = json_object_get(params, "prefetch");
    if (prefetch_val && prefetch_val->type == JSON_NUMBER) {
        prefetch = (uint32_t)prefetch_val->u.number;
        if (prefetch == 0) prefetch = 1;
    }
    
    json_value_t *exclusive_val = json_object_get(params, "exclusive");
    if (exclusive_val && exclusive_val->type == JSON_BOOL) {
        exclusive = exclusive_val->u.boolean;
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(queue_name);
    if (!q) {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "queue not found", NULL);
        return;
    }
    
    consumer_t *consumer = calloc(1, sizeof(consumer_t));
    if (!consumer) {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "out of memory", NULL);
        return;
    }
    
    snprintf(consumer->consumer_id, sizeof(consumer->consumer_id), 
             "consumer-%lu", __sync_fetch_and_add(&g_mq.next_consumer_id, 1));
    strncpy(consumer->queue_name, queue_name, MAX_QUEUE_NAME - 1);
    consumer->client_fd = ctx->client_fd;
    consumer->active = true;
    consumer->exclusive = exclusive;
    consumer->prefetch_count = prefetch;
    consumer->unacked_count = 0;
    
    g_mq.consumers[g_mq.consumer_count++] = consumer;
    q->consumer_count++;
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_OBJECT;
    result->u.object.keys = malloc(sizeof(char*));
    result->u.object.values = malloc(sizeof(json_value_t*));
    result->u.object.count = 1;
    
    result->u.object.keys[0] = strdup("consumer_id");
    result->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[0]->type = JSON_STRING;
    result->u.object.values[0]->u.string = strdup(consumer->consumer_id);
    
    send_json_response(ctx->client_fd, "ok", "consumer registered", result);
    json_free(result);
}

static void handle_get(client_context_t *ctx, json_value_t *params) {
    json_value_t *queue_val = json_object_get(params, "queue");
    if (!queue_val || queue_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue name", NULL);
        return;
    }
    
    const char *queue_name = json_as_string(queue_val);
    bool no_ack = false;
    
    json_value_t *no_ack_val = json_object_get(params, "no_ack");
    if (no_ack_val && no_ack_val->type == JSON_BOOL) {
        no_ack = no_ack_val->u.boolean;
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(queue_name);
    if (!q) {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "queue not found", NULL);
        return;
    }
    
    pthread_mutex_lock(&q->lock);
    pthread_mutex_unlock(&g_mq.global_lock);
    
    purge_expired_messages(q);
    
    message_t *msg = NULL;
    list_node_t *node = q->messages->head;
    while (node) {
        msg = (message_t*)node->data;
        if (msg->status == MSG_STATUS_PENDING) {
            break;
        }
        msg = NULL;
        node = node->next;
    }
    
    if (!msg) {
        pthread_mutex_unlock(&q->lock);
        send_json_response(ctx->client_fd, "ok", "no messages available", NULL);
        return;
    }
    
    msg->status = no_ack ? MSG_STATUS_ACKNOWLEDGED : MSG_STATUS_DELIVERED;
    msg->delivery_count++;
    q->total_consumed++;
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_OBJECT;
    result->u.object.keys = malloc(6 * sizeof(char*));
    result->u.object.values = malloc(6 * sizeof(json_value_t*));
    result->u.object.count = 6;
    
    result->u.object.keys[0] = strdup("message_id");
    result->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[0]->type = JSON_NUMBER;
    result->u.object.values[0]->u.number = (double)msg->id;
    
    result->u.object.keys[1] = strdup("body");
    result->u.object.values[1] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[1]->type = JSON_STRING;
    result->u.object.values[1]->u.string = strdup(msg->body);
    
    result->u.object.keys[2] = strdup("priority");
    result->u.object.values[2] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[2]->type = JSON_NUMBER;
    result->u.object.values[2]->u.number = msg->priority;
    
    result->u.object.keys[3] = strdup("timestamp");
    result->u.object.values[3] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[3]->type = JSON_NUMBER;
    result->u.object.values[3]->u.number = (double)msg->timestamp;
    
    result->u.object.keys[4] = strdup("content_type");
    result->u.object.values[4] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[4]->type = JSON_STRING;
    result->u.object.values[4]->u.string = strdup(msg->content_type);
    
    result->u.object.keys[5] = strdup("delivery_tag");
    result->u.object.values[5] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[5]->type = JSON_NUMBER;
    result->u.object.values[5]->u.number = (double)msg->id;
    
    pthread_mutex_unlock(&q->lock);
    
    send_json_response(ctx->client_fd, "ok", "", result);
    json_free(result);
}

static void handle_ack(client_context_t *ctx, json_value_t *params) {
    json_value_t *delivery_tag_val = json_object_get(params, "delivery_tag");
    if (!delivery_tag_val || delivery_tag_val->type != JSON_NUMBER) {
        send_json_response(ctx->client_fd, "error", "missing delivery_tag", NULL);
        return;
    }
    
    uint64_t delivery_tag = (uint64_t)delivery_tag_val->u.number;
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    bool found = false;
    for (size_t i = 0; i < g_mq.queue_count && !found; i++) {
        queue_t *q = g_mq.queues[i];
        pthread_mutex_lock(&q->lock);
        
        list_node_t *node = q->messages->head;
        while (node) {
            message_t *msg = (message_t*)node->data;
            if (msg->id == delivery_tag) {
                if (msg->status == MSG_STATUS_DELIVERED) {
                    msg->status = MSG_STATUS_ACKNOWLEDGED;
                    q->total_acked++;
                    
                    list_remove_node(q->messages, node);
                    free_message(msg);
                    q->message_count--;
                }
                found = true;
                break;
            }
            node = node->next;
        }
        
        pthread_mutex_unlock(&q->lock);
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    if (found) {
        send_json_response(ctx->client_fd, "ok", "message acknowledged", NULL);
    } else {
        send_json_response(ctx->client_fd, "error", "message not found", NULL);
    }
}

static void handle_nack(client_context_t *ctx, json_value_t *params) {
    json_value_t *delivery_tag_val = json_object_get(params, "delivery_tag");
    if (!delivery_tag_val || delivery_tag_val->type != JSON_NUMBER) {
        send_json_response(ctx->client_fd, "error", "missing delivery_tag", NULL);
        return;
    }
    
    uint64_t delivery_tag = (uint64_t)delivery_tag_val->u.number;
    bool requeue = true;
    
    json_value_t *requeue_val = json_object_get(params, "requeue");
    if (requeue_val && requeue_val->type == JSON_BOOL) {
        requeue = requeue_val->u.boolean;
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    bool found = false;
    for (size_t i = 0; i < g_mq.queue_count && !found; i++) {
        queue_t *q = g_mq.queues[i];
        pthread_mutex_lock(&q->lock);
        
        list_node_t *node = q->messages->head;
        while (node) {
            message_t *msg = (message_t*)node->data;
            if (msg->id == delivery_tag) {
                if (msg->status == MSG_STATUS_DELIVERED) {
                    q->total_nacked++;
                    
                    if (requeue) {
                        msg->status = MSG_STATUS_PENDING;
                    } else {
                        msg->status = MSG_STATUS_REJECTED;
                        list_remove_node(q->messages, node);
                        free_message(msg);
                        q->message_count--;
                    }
                }
                found = true;
                break;
            }
            node = node->next;
        }
        
        pthread_mutex_unlock(&q->lock);
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    if (found) {
        send_json_response(ctx->client_fd, "ok", "message nacked", NULL);
    } else {
        send_json_response(ctx->client_fd, "error", "message not found", NULL);
    }
}

static void handle_queue_status(client_context_t *ctx, json_value_t *params) {
    json_value_t *queue_val = json_object_get(params, "queue");
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_ARRAY;
    result->u.array.items = malloc(g_mq.queue_count * sizeof(json_value_t*));
    result->u.array.count = 0;
    
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        queue_t *q = g_mq.queues[i];
        
        if (queue_val && queue_val->type == JSON_STRING) {
            if (strcmp(q->name, json_as_string(queue_val)) != 0) continue;
        }
        
        pthread_mutex_lock(&q->lock);
        
        json_value_t *q_info = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->type = JSON_OBJECT;
        q_info->u.object.keys = malloc(10 * sizeof(char*));
        q_info->u.object.values = malloc(10 * sizeof(json_value_t*));
        q_info->u.object.count = 10;
        
        q_info->u.object.keys[0] = strdup("name");
        q_info->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[0]->type = JSON_STRING;
        q_info->u.object.values[0]->u.string = strdup(q->name);
        
        q_info->u.object.keys[1] = strdup("message_count");
        q_info->u.object.values[1] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[1]->type = JSON_NUMBER;
        q_info->u.object.values[1]->u.number = (double)q->message_count;
        
        q_info->u.object.keys[2] = strdup("consumer_count");
        q_info->u.object.values[2] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[2]->type = JSON_NUMBER;
        q_info->u.object.values[2]->u.number = (double)q->consumer_count;
        
        q_info->u.object.keys[3] = strdup("total_published");
        q_info->u.object.values[3] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[3]->type = JSON_NUMBER;
        q_info->u.object.values[3]->u.number = (double)q->total_published;
        
        q_info->u.object.keys[4] = strdup("total_consumed");
        q_info->u.object.values[4] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[4]->type = JSON_NUMBER;
        q_info->u.object.values[4]->u.number = (double)q->total_consumed;
        
        q_info->u.object.keys[5] = strdup("total_acked");
        q_info->u.object.values[5] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[5]->type = JSON_NUMBER;
        q_info->u.object.values[5]->u.number = (double)q->total_acked;
        
        q_info->u.object.keys[6] = strdup("total_nacked");
        q_info->u.object.values[6] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[6]->type = JSON_NUMBER;
        q_info->u.object.values[6]->u.number = (double)q->total_nacked;
        
        q_info->u.object.keys[7] = strdup("durable");
        q_info->u.object.values[7] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[7]->type = JSON_BOOL;
        q_info->u.object.values[7]->u.boolean = q->durable;
        
        q_info->u.object.keys[8] = strdup("capacity");
        q_info->u.object.values[8] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[8]->type = JSON_NUMBER;
        q_info->u.object.values[8]->u.number = (double)q->capacity;
        
        q_info->u.object.keys[9] = strdup("max_length");
        q_info->u.object.values[9] = (json_value_t*)calloc(1, sizeof(json_value_t));
        q_info->u.object.values[9]->type = JSON_NUMBER;
        q_info->u.object.values[9]->u.number = (double)q->max_length;
        
        pthread_mutex_unlock(&q->lock);
        
        result->u.array.items[result->u.array.count++] = q_info;
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    send_json_response(ctx->client_fd, "ok", "", result);
    json_free(result);
}

static void handle_server_status(client_context_t *ctx) {
    pthread_mutex_lock(&g_mq.global_lock);
    
    time_t now = time(NULL);
    time_t uptime = now - g_mq.start_time;
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_OBJECT;
    result->u.object.keys = malloc(8 * sizeof(char*));
    result->u.object.values = malloc(8 * sizeof(json_value_t*));
    result->u.object.count = 8;
    
    result->u.object.keys[0] = strdup("uptime_seconds");
    result->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[0]->type = JSON_NUMBER;
    result->u.object.values[0]->u.number = (double)uptime;
    
    result->u.object.keys[1] = strdup("queue_count");
    result->u.object.values[1] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[1]->type = JSON_NUMBER;
    result->u.object.values[1]->u.number = (double)g_mq.queue_count;
    
    result->u.object.keys[2] = strdup("consumer_count");
    result->u.object.values[2] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[2]->type = JSON_NUMBER;
    result->u.object.values[2]->u.number = (double)g_mq.consumer_count;
    
    result->u.object.keys[3] = strdup("total_connections");
    result->u.object.values[3] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[3]->type = JSON_NUMBER;
    result->u.object.values[3]->u.number = (double)g_mq.total_connections;
    
    result->u.object.keys[4] = strdup("active_connections");
    result->u.object.values[4] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[4]->type = JSON_NUMBER;
    result->u.object.values[4]->u.number = (double)g_mq.active_connections;
    
    result->u.object.keys[5] = strdup("total_messages_published");
    result->u.object.values[5] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[5]->type = JSON_NUMBER;
    
    uint64_t total_published = 0;
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        total_published += g_mq.queues[i]->total_published;
    }
    result->u.object.values[5]->u.number = (double)total_published;
    
    result->u.object.keys[6] = strdup("worker_threads");
    result->u.object.values[6] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[6]->type = JSON_NUMBER;
    result->u.object.values[6]->u.number = (double)threadpool_get_thread_count(g_mq.pool);
    
    result->u.object.keys[7] = strdup("next_message_id");
    result->u.object.values[7] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[7]->type = JSON_NUMBER;
    result->u.object.values[7]->u.number = (double)g_mq.next_message_id;
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    send_json_response(ctx->client_fd, "ok", "", result);
    json_free(result);
}

static void handle_purge_queue(client_context_t *ctx, json_value_t *params) {
    json_value_t *queue_val = json_object_get(params, "queue");
    if (!queue_val || queue_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing queue name", NULL);
        return;
    }
    
    const char *queue_name = json_as_string(queue_val);
    
    pthread_mutex_lock(&g_mq.global_lock);
    
    queue_t *q = find_queue(queue_name);
    if (!q) {
        pthread_mutex_unlock(&g_mq.global_lock);
        send_json_response(ctx->client_fd, "error", "queue not found", NULL);
        return;
    }
    
    pthread_mutex_lock(&q->lock);
    pthread_mutex_unlock(&g_mq.global_lock);
    
    size_t purged = q->message_count;
    list_free_with_data(q->messages, free_message);
    q->messages = list_create();
    q->message_count = 0;
    
    pthread_mutex_unlock(&q->lock);
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_OBJECT;
    result->u.object.keys = malloc(sizeof(char*));
    result->u.object.values = malloc(sizeof(json_value_t*));
    result->u.object.count = 1;
    
    result->u.object.keys[0] = strdup("purged_count");
    result->u.object.values[0] = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->u.object.values[0]->type = JSON_NUMBER;
    result->u.object.values[0]->u.number = (double)purged;
    
    send_json_response(ctx->client_fd, "ok", "queue purged", result);
    json_free(result);
}

static void handle_list_queues(client_context_t *ctx) {
    pthread_mutex_lock(&g_mq.global_lock);
    
    json_value_t *result = (json_value_t*)calloc(1, sizeof(json_value_t));
    result->type = JSON_ARRAY;
    result->u.array.items = malloc(g_mq.queue_count * sizeof(json_value_t*));
    result->u.array.count = g_mq.queue_count;
    
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        result->u.array.items[i] = (json_value_t*)calloc(1, sizeof(json_value_t));
        result->u.array.items[i]->type = JSON_STRING;
        result->u.array.items[i]->u.string = strdup(g_mq.queues[i]->name);
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    send_json_response(ctx->client_fd, "ok", "", result);
    json_free(result);
}

static void process_command(client_context_t *ctx, const char *buffer) {
    json_value_t *cmd = json_parse(buffer);
    if (!cmd || cmd->type != JSON_OBJECT) {
        send_json_response(ctx->client_fd, "error", "invalid JSON", NULL);
        if (cmd) json_free(cmd);
        return;
    }
    
    json_value_t *action_val = json_object_get(cmd, "action");
    if (!action_val || action_val->type != JSON_STRING) {
        send_json_response(ctx->client_fd, "error", "missing action", NULL);
        json_free(cmd);
        return;
    }
    
    json_value_t *params = json_object_get(cmd, "params");
    if (!params) params = cmd;
    
    const char *action = json_as_string(action_val);
    
    if (strcmp(action, "declare_queue") == 0) {
        handle_declare_queue(ctx, params);
    } else if (strcmp(action, "delete_queue") == 0) {
        handle_delete_queue(ctx, params);
    } else if (strcmp(action, "publish") == 0) {
        handle_publish(ctx, params);
    } else if (strcmp(action, "consume") == 0) {
        handle_consume(ctx, params);
    } else if (strcmp(action, "get") == 0) {
        handle_get(ctx, params);
    } else if (strcmp(action, "ack") == 0) {
        handle_ack(ctx, params);
    } else if (strcmp(action, "nack") == 0) {
        handle_nack(ctx, params);
    } else if (strcmp(action, "queue_status") == 0) {
        handle_queue_status(ctx, params);
    } else if (strcmp(action, "server_status") == 0) {
        handle_server_status(ctx);
    } else if (strcmp(action, "purge_queue") == 0) {
        handle_purge_queue(ctx, params);
    } else if (strcmp(action, "list_queues") == 0) {
        handle_list_queues(ctx);
    } else if (strcmp(action, "ping") == 0) {
        send_json_response(ctx->client_fd, "ok", "pong", NULL);
    } else {
        send_json_response(ctx->client_fd, "error", "unknown action", NULL);
    }
    
    json_free(cmd);
}

static void handle_client(void *arg) {
    client_context_t *ctx = (client_context_t*)arg;
    char buffer[8192];
    char line_buffer[16384];
    size_t line_len = 0;
    
    printf("Client connected from %s\n", ctx->client_ip);
    
    pthread_mutex_lock(&g_mq.global_lock);
    g_mq.active_connections++;
    pthread_mutex_unlock(&g_mq.global_lock);
    
    while (g_mq.running) {
        int n = net_recv(ctx->client_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        
        buffer[n] = '\0';
        
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                line_buffer[line_len] = '\0';
                
                if (line_len > 0) {
                    process_command(ctx, line_buffer);
                }
                
                line_len = 0;
            } else {
                if (line_len < sizeof(line_buffer) - 1) {
                    line_buffer[line_len++] = buffer[i];
                }
            }
        }
    }
    
    pthread_mutex_lock(&g_mq.global_lock);
    g_mq.active_connections--;
    
    for (size_t i = 0; i < g_mq.consumer_count; ) {
        if (g_mq.consumers[i]->client_fd == ctx->client_fd) {
            free(g_mq.consumers[i]);
            memmove(&g_mq.consumers[i], &g_mq.consumers[i + 1],
                    (g_mq.consumer_count - i - 1) * sizeof(consumer_t*));
            g_mq.consumer_count--;
        } else {
            i++;
        }
    }
    
    pthread_mutex_unlock(&g_mq.global_lock);
    
    net_close(ctx->client_fd);
    printf("Client disconnected from %s\n", ctx->client_ip);
    free(ctx);
}

static void signal_handler(int sig) {
    (void)sig;
    g_mq.running = false;
    printf("\nShutting down server...\n");
}

static void print_banner(void) {
    term_printf(TERM_ANSI_CYAN,
        "╔════════════════════════════════════════════════════════════╗\n"
        "║                  消息队列系统 v1.0                          ║\n"
        "╠════════════════════════════════════════════════════════════╣\n"
        "║  基于 c_utils 库构建                                       ║\n"
        "║  支持多生产者/消费者模式                                    ║\n"
        "╚════════════════════════════════════════════════════════════╝\n"
        TERM_ANSI_RESET);
}

static void print_help(const char *prog) {
    print_banner();
    printf("\n用法: %s [选项]\n\n", prog);
    printf("选项:\n");
    printf("  -p, --port <port>      监听端口 (默认: %s)\n", DEFAULT_PORT);
    printf("  -t, --threads <num>    工作线程数 (默认: CPU核心数)\n");
    printf("  -h, --help             显示帮助信息\n");
    printf("\n支持的命令 (JSON格式):\n");
    printf("  declare_queue  - 声明队列\n");
    printf("  delete_queue   - 删除队列\n");
    printf("  publish        - 发布消息\n");
    printf("  consume        - 注册消费者\n");
    printf("  get            - 获取消息\n");
    printf("  ack            - 确认消息\n");
    printf("  nack           - 拒绝消息\n");
    printf("  queue_status   - 队列状态\n");
    printf("  server_status  - 服务器状态\n");
    printf("  purge_queue    - 清空队列\n");
    printf("  list_queues    - 列出所有队列\n");
    printf("  ping           - 测试连接\n");
}

int main(int argc, char *argv[]) {
    const char *port = DEFAULT_PORT;
    int num_threads = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) port = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
            if (i + 1 < argc) num_threads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
    }
    
    memset(&g_mq, 0, sizeof(g_mq));
    g_mq.running = true;
    g_mq.start_time = time(NULL);
    pthread_mutex_init(&g_mq.global_lock, NULL);
    
    g_mq.pool = threadpool_create(num_threads);
    if (!g_mq.pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return 1;
    }
    
    if (!net_init()) {
        fprintf(stderr, "Failed to initialize network\n");
        threadpool_destroy(g_mq.pool);
        return 1;
    }
    
    g_mq.server_fd = net_listen(port);
    if (g_mq.server_fd == INVALID_SOCKET) {
        fprintf(stderr, "Failed to listen on port %s\n", port);
        net_cleanup();
        threadpool_destroy(g_mq.pool);
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    print_banner();
    printf("\n服务器启动:\n");
    printf("  端口: %s\n", port);
    printf("  工作线程: %d\n", threadpool_get_thread_count(g_mq.pool));
    printf("\n等待客户端连接...\n");
    printf("使用 nc localhost %s 连接\n\n", port);
    
    while (g_mq.running) {
        socket_t client_fd = net_accept(g_mq.server_fd);
        
        if (client_fd == INVALID_SOCKET) {
            if (g_mq.running) usleep(10000);
            continue;
        }
        
        client_context_t *ctx = malloc(sizeof(client_context_t));
        if (!ctx) {
            net_close(client_fd);
            continue;
        }
        
        ctx->client_fd = client_fd;
        strncpy(ctx->client_ip, "unknown", sizeof(ctx->client_ip) - 1);
        
        pthread_mutex_lock(&g_mq.global_lock);
        g_mq.total_connections++;
        pthread_mutex_unlock(&g_mq.global_lock);
        
        threadpool_add_task(g_mq.pool, handle_client, ctx);
    }
    
    printf("正在关闭服务器...\n");
    
    net_close(g_mq.server_fd);
    threadpool_wait_all(g_mq.pool, 5000);
    threadpool_destroy(g_mq.pool);
    
    for (size_t i = 0; i < g_mq.queue_count; i++) {
        delete_queue(g_mq.queues[i]);
    }
    
    for (size_t i = 0; i < g_mq.consumer_count; i++) {
        free(g_mq.consumers[i]);
    }
    
    pthread_mutex_destroy(&g_mq.global_lock);
    net_cleanup();
    
    printf("服务器已关闭\n");
    return 0;
}
