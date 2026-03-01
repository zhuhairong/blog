#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include "argparse.h"
#include "json.h"
#include "hashmap.h"
#include "list.h"
#include "time_utils.h"
#include "fs_utils.h"
#include "terminal.h"

#define MAX_TITLE_LEN 256
#define MAX_DESC_LEN 512
#define MAX_TAG_LEN 64
#define MAX_TAGS 10
#define DATA_FILE "tasks.json"

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_MEDIUM = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_URGENT = 3
} task_priority_t;

typedef enum {
    STATUS_PENDING = 0,
    STATUS_IN_PROGRESS = 1,
    STATUS_COMPLETED = 2,
    STATUS_CANCELLED = 3
} task_status_t;

typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    char description[MAX_DESC_LEN];
    char tags[MAX_TAGS][MAX_TAG_LEN];
    int tag_count;
    task_priority_t priority;
    task_status_t status;
    time_t created_at;
    time_t updated_at;
    time_t due_date;
    time_t completed_at;
} task_t;

typedef struct {
    list_t *tasks;
    hashmap_t *by_id;
    int next_id;
    char data_file[256];
} task_manager_t;

static const char* priority_names[] = {"低", "中", "高", "紧急"};
static const char* status_names[] = {"待办", "进行中", "已完成", "已取消"};
static const char* priority_colors[] = {"\033[37m", "\033[33m", "\033[31m", "\033[35m"};
static const char* status_colors[] = {"\033[36m", "\033[33m", "\033[32m", "\033[90m"};

static task_manager_t* tm_create(const char *data_file) {
    task_manager_t *tm = calloc(1, sizeof(task_manager_t));
    if (!tm) return NULL;
    
    tm->tasks = list_create();
    tm->by_id = hashmap_create();
    tm->next_id = 1;
    
    if (data_file) {
        strncpy(tm->data_file, data_file, sizeof(tm->data_file) - 1);
    } else {
        strncpy(tm->data_file, DATA_FILE, sizeof(tm->data_file) - 1);
    }
    
    return tm;
}

static void tm_free_task(void *data) {
    if (data) free(data);
}

static void tm_free(task_manager_t *tm) {
    if (!tm) return;
    list_free_with_data(tm->tasks, tm_free_task);
    hashmap_free(tm->by_id);
    free(tm);
}

static task_t* tm_create_task(void) {
    task_t *task = calloc(1, sizeof(task_t));
    if (!task) return NULL;
    
    task->created_at = time(NULL);
    task->updated_at = task->created_at;
    task->status = STATUS_PENDING;
    task->priority = PRIORITY_MEDIUM;
    task->due_date = 0;
    task->completed_at = 0;
    task->tag_count = 0;
    
    return task;
}

static bool tm_add_task(task_manager_t *tm, task_t *task) {
    if (!tm || !task) return false;
    
    task->id = tm->next_id++;
    task->updated_at = time(NULL);
    
    list_push_back(tm->tasks, task);
    
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", task->id);
    hashmap_set(tm->by_id, id_str, task);
    
    return true;
}

static task_t* tm_get_task(task_manager_t *tm, int id) {
    if (!tm) return NULL;
    
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", id);
    return (task_t*)hashmap_get(tm->by_id, id_str);
}

static bool tm_remove_task(task_manager_t *tm, int id) {
    if (!tm) return false;
    
    task_t *task = tm_get_task(tm, id);
    if (!task) return false;
    
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", id);
    hashmap_remove(tm->by_id, id_str);
    
    list_node_t *node;
    list_foreach(tm->tasks, node) {
        if (node->data == task) {
            list_remove_node(tm->tasks, node);
            free(task);
            return true;
        }
    }
    
    return false;
}

static bool tm_update_status(task_manager_t *tm, int id, task_status_t status) {
    task_t *task = tm_get_task(tm, id);
    if (!task) return false;
    
    task->status = status;
    task->updated_at = time(NULL);
    
    if (status == STATUS_COMPLETED) {
        task->completed_at = time(NULL);
    }
    
    return true;
}

static bool tm_update_priority(task_manager_t *tm, int id, task_priority_t priority) {
    task_t *task = tm_get_task(tm, id);
    if (!task) return false;
    
    task->priority = priority;
    task->updated_at = time(NULL);
    return true;
}

static bool tm_set_due_date(task_manager_t *tm, int id, time_t due_date) {
    task_t *task = tm_get_task(tm, id);
    if (!task) return false;
    
    task->due_date = due_date;
    task->updated_at = time(NULL);
    return true;
}

static bool tm_add_tag(task_manager_t *tm, int id, const char *tag) {
    task_t *task = tm_get_task(tm, id);
    if (!task || !tag) return false;
    
    if (task->tag_count >= MAX_TAGS) return false;
    
    for (int i = 0; i < task->tag_count; i++) {
        if (strcmp(task->tags[i], tag) == 0) return false;
    }
    
    strncpy(task->tags[task->tag_count], tag, MAX_TAG_LEN - 1);
    task->tag_count++;
    task->updated_at = time(NULL);
    return true;
}

static void json_add_number(json_value_t *obj, const char *key, double val) {
    size_t idx = obj->u.object.count;
    obj->u.object.keys[idx] = strdup(key);
    json_value_t *v = calloc(1, sizeof(json_value_t));
    v->type = JSON_NUMBER;
    v->u.number = val;
    obj->u.object.values[idx] = v;
    obj->u.object.count++;
}

static void json_add_string(json_value_t *obj, const char *key, const char *val) {
    size_t idx = obj->u.object.count;
    obj->u.object.keys[idx] = strdup(key);
    json_value_t *v = calloc(1, sizeof(json_value_t));
    v->type = JSON_STRING;
    v->u.string = strdup(val);
    obj->u.object.values[idx] = v;
    obj->u.object.count++;
}

static json_value_t* tm_task_to_json(const task_t *task) {
    json_value_t *obj = calloc(1, sizeof(json_value_t));
    obj->type = JSON_OBJECT;
    obj->u.object.keys = calloc(20, sizeof(char*));
    obj->u.object.values = calloc(20, sizeof(json_value_t*));
    obj->u.object.count = 0;
    
    json_add_number(obj, "id", task->id);
    json_add_string(obj, "title", task->title);
    json_add_string(obj, "description", task->description);
    json_add_number(obj, "priority", task->priority);
    json_add_number(obj, "status", task->status);
    json_add_number(obj, "created_at", task->created_at);
    json_add_number(obj, "updated_at", task->updated_at);
    json_add_number(obj, "due_date", task->due_date);
    json_add_number(obj, "completed_at", task->completed_at);
    
    return obj;
}

static bool tm_save(task_manager_t *tm) {
    if (!tm || !tm->data_file[0]) return false;
    
    json_value_t *root = calloc(1, sizeof(json_value_t));
    root->type = JSON_ARRAY;
    root->u.array.items = calloc(list_size(tm->tasks), sizeof(json_value_t*));
    root->u.array.count = 0;
    
    list_node_t *node;
    list_foreach(tm->tasks, node) {
        task_t *task = (task_t*)node->data;
        root->u.array.items[root->u.array.count++] = tm_task_to_json(task);
    }
    
    char *json_str = json_print(root);
    json_free(root);
    
    if (!json_str) return false;
    
    FILE *fp = fopen(tm->data_file, "w");
    if (!fp) {
        free(json_str);
        return false;
    }
    
    fprintf(fp, "%s\n", json_str);
    fclose(fp);
    free(json_str);
    
    return true;
}

static task_t* tm_task_from_json(const json_value_t *obj) {
    if (!obj || obj->type != JSON_OBJECT) return NULL;
    
    task_t *task = tm_create_task();
    if (!task) return NULL;
    
    json_value_t *v;
    
    v = json_object_get(obj, "id");
    if (v) task->id = (int)json_as_number(v);
    
    v = json_object_get(obj, "title");
    if (v) strncpy(task->title, json_as_string(v), MAX_TITLE_LEN - 1);
    
    v = json_object_get(obj, "description");
    if (v) strncpy(task->description, json_as_string(v), MAX_DESC_LEN - 1);
    
    v = json_object_get(obj, "priority");
    if (v) task->priority = (task_priority_t)(int)json_as_number(v);
    
    v = json_object_get(obj, "status");
    if (v) task->status = (task_status_t)(int)json_as_number(v);
    
    v = json_object_get(obj, "created_at");
    if (v) task->created_at = (time_t)json_as_number(v);
    
    v = json_object_get(obj, "updated_at");
    if (v) task->updated_at = (time_t)json_as_number(v);
    
    v = json_object_get(obj, "due_date");
    if (v) task->due_date = (time_t)json_as_number(v);
    
    v = json_object_get(obj, "completed_at");
    if (v) task->completed_at = (time_t)json_as_number(v);
    
    return task;
}

static bool tm_load(task_manager_t *tm) {
    if (!tm || !tm->data_file[0]) return false;
    
    FILE *fp = fopen(tm->data_file, "r");
    if (!fp) return false;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return false;
    }
    
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);
    
    json_value_t *root = json_parse(buffer);
    free(buffer);
    
    if (!root || root->type != JSON_ARRAY) {
        if (root) json_free(root);
        return false;
    }
    
    int max_id = 0;
    for (size_t i = 0; i < root->u.array.count; i++) {
        task_t *task = tm_task_from_json(root->u.array.items[i]);
        if (task) {
            list_push_back(tm->tasks, task);
            
            char id_str[32];
            snprintf(id_str, sizeof(id_str), "%d", task->id);
            hashmap_set(tm->by_id, id_str, task);
            
            if (task->id > max_id) max_id = task->id;
        }
    }
    
    tm->next_id = max_id + 1;
    json_free(root);
    return true;
}

static void print_task(const task_t *task, bool detailed) {
    char created_str[32] = {0};
    char updated_str[32] = {0};
    char due_str[32] = {0};
    
    struct tm *tm_info = localtime(&task->created_at);
    strftime(created_str, sizeof(created_str), "%Y-%m-%d %H:%M", tm_info);
    
    tm_info = localtime(&task->updated_at);
    strftime(updated_str, sizeof(updated_str), "%Y-%m-%d %H:%M", tm_info);
    
    if (task->due_date > 0) {
        tm_info = localtime(&task->due_date);
        strftime(due_str, sizeof(due_str), "%Y-%m-%d", tm_info);
    }
    
    printf("%s[%d]\033[0m ", task->id < 10 ? " " : "", task->id);
    printf("%s%-8s\033[0m ", status_colors[task->status], status_names[task->status]);
    printf("%s%-6s\033[0m ", priority_colors[task->priority], priority_names[task->priority]);
    printf("%s\033[0m", task->title);
    
    if (task->due_date > 0) {
        time_t now = time(NULL);
        if (task->due_date < now && task->status != STATUS_COMPLETED) {
            printf(" \033[31m[逾期]\033[0m");
        }
        printf(" (截止: %s)", due_str);
    }
    
    printf("\n");
    
    if (detailed) {
        if (task->description[0]) {
            printf("    描述: %s\n", task->description);
        }
        if (task->tag_count > 0) {
            printf("    标签: ");
            for (int i = 0; i < task->tag_count; i++) {
                printf("#%s ", task->tags[i]);
            }
            printf("\n");
        }
        printf("    创建: %s | 更新: %s\n", created_str, updated_str);
        if (task->status == STATUS_COMPLETED && task->completed_at > 0) {
            char completed_str[32] = {0};
            tm_info = localtime(&task->completed_at);
            strftime(completed_str, sizeof(completed_str), "%Y-%m-%d %H:%M", tm_info);
            printf("    完成: %s\n", completed_str);
        }
    }
}

static void print_task_list(task_manager_t *tm, task_status_t filter_status, const char *filter_tag) {
    if (!tm || list_is_empty(tm->tasks)) {
        printf("没有任务\n");
        return;
    }
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                        任务列表                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    int count = 0;
    list_node_t *node;
    list_foreach(tm->tasks, node) {
        task_t *task = (task_t*)node->data;
        
        if (filter_status != -1 && task->status != filter_status) continue;
        
        if (filter_tag && filter_tag[0]) {
            bool found = false;
            for (int i = 0; i < task->tag_count; i++) {
                if (strcmp(task->tags[i], filter_tag) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) continue;
        }
        
        print_task(task, false);
        count++;
    }
    
    printf("\n共 %d 个任务\n", count);
}

static void print_task_detail(task_manager_t *tm, int id) {
    task_t *task = tm_get_task(tm, id);
    if (!task) {
        printf("任务 #%d 不存在\n", id);
        return;
    }
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      任务详情 #%d                            ║\n", id);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    print_task(task, true);
}

static void print_statistics(task_manager_t *tm) {
    int total = 0, pending = 0, in_progress = 0, completed = 0, cancelled = 0;
    int overdue = 0;
    time_t now = time(NULL);
    
    list_node_t *node;
    list_foreach(tm->tasks, node) {
        task_t *task = (task_t*)node->data;
        total++;
        
        switch (task->status) {
            case STATUS_PENDING: pending++; break;
            case STATUS_IN_PROGRESS: in_progress++; break;
            case STATUS_COMPLETED: completed++; break;
            case STATUS_CANCELLED: cancelled++; break;
        }
        
        if (task->due_date > 0 && task->due_date < now && task->status != STATUS_COMPLETED) {
            overdue++;
        }
    }
    
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                        任务统计                              ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  总计: %-4d  |  待办: %-4d  |  进行中: %-4d               ║\n", total, pending, in_progress);
    printf("║  完成: %-4d  |  取消: %-4d  |  逾期: %-4d                ║\n", completed, cancelled, overdue);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

static void print_help(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     任务管理器帮助                           ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  task add <标题>              添加新任务                     ║\n");
    printf("║  task list                    列出所有任务                   ║\n");
    printf("║  task show <ID>               显示任务详情                   ║\n");
    printf("║  task done <ID>               标记任务完成                   ║\n");
    printf("║  task start <ID>              开始任务                       ║\n");
    printf("║  task cancel <ID>             取消任务                       ║\n");
    printf("║  task rm <ID>                 删除任务                       ║\n");
    printf("║  task priority <ID> <级别>    设置优先级(低/中/高/紧急)      ║\n");
    printf("║  task due <ID> <日期>         设置截止日期(YYYY-MM-DD)       ║\n");
    printf("║  task tag <ID> <标签>         添加标签                       ║\n");
    printf("║  task stats                   显示统计                       ║\n");
    printf("║  task help                    显示帮助                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

static time_t parse_date(const char *date_str) {
    struct tm tm = {0};
    if (sscanf(date_str, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3) {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        return mktime(&tm);
    }
    return 0;
}

static task_priority_t parse_priority(const char *str) {
    if (strcmp(str, "低") == 0 || strcmp(str, "low") == 0) return PRIORITY_LOW;
    if (strcmp(str, "中") == 0 || strcmp(str, "medium") == 0) return PRIORITY_MEDIUM;
    if (strcmp(str, "高") == 0 || strcmp(str, "high") == 0) return PRIORITY_HIGH;
    if (strcmp(str, "紧急") == 0 || strcmp(str, "urgent") == 0) return PRIORITY_URGENT;
    return PRIORITY_MEDIUM;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    task_manager_t *tm = tm_create(NULL);
    if (!tm) {
        fprintf(stderr, "无法初始化任务管理器\n");
        return 1;
    }
    
    tm_load(tm);
    
    const char *cmd = argv[1];
    
    if (strcmp(cmd, "add") == 0 && argc >= 3) {
        task_t *task = tm_create_task();
        strncpy(task->title, argv[2], MAX_TITLE_LEN - 1);
        
        if (argc >= 4) {
            strncpy(task->description, argv[3], MAX_DESC_LEN - 1);
        }
        
        tm_add_task(tm, task);
        tm_save(tm);
        printf("已添加任务 #%d: %s\n", task->id, task->title);
        
    } else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "ls") == 0) {
        task_status_t filter = -1;
        if (argc >= 3) {
            if (strcmp(argv[2], "done") == 0) filter = STATUS_COMPLETED;
            else if (strcmp(argv[2], "pending") == 0) filter = STATUS_PENDING;
            else if (strcmp(argv[2], "progress") == 0) filter = STATUS_IN_PROGRESS;
        }
        print_task_list(tm, filter, NULL);
        
    } else if (strcmp(cmd, "show") == 0 && argc >= 3) {
        int id = atoi(argv[2]);
        print_task_detail(tm, id);
        
    } else if (strcmp(cmd, "done") == 0 && argc >= 3) {
        int id = atoi(argv[2]);
        if (tm_update_status(tm, id, STATUS_COMPLETED)) {
            tm_save(tm);
            printf("任务 #%d 已完成\n", id);
        } else {
            printf("任务 #%d 不存在\n", id);
        }
        
    } else if (strcmp(cmd, "start") == 0 && argc >= 3) {
        int id = atoi(argv[2]);
        if (tm_update_status(tm, id, STATUS_IN_PROGRESS)) {
            tm_save(tm);
            printf("任务 #%d 已开始\n", id);
        } else {
            printf("任务 #%d 不存在\n", id);
        }
        
    } else if (strcmp(cmd, "cancel") == 0 && argc >= 3) {
        int id = atoi(argv[2]);
        if (tm_update_status(tm, id, STATUS_CANCELLED)) {
            tm_save(tm);
            printf("任务 #%d 已取消\n", id);
        } else {
            printf("任务 #%d 不存在\n", id);
        }
        
    } else if (strcmp(cmd, "rm") == 0 && argc >= 3) {
        int id = atoi(argv[2]);
        if (tm_remove_task(tm, id)) {
            tm_save(tm);
            printf("任务 #%d 已删除\n", id);
        } else {
            printf("任务 #%d 不存在\n", id);
        }
        
    } else if (strcmp(cmd, "priority") == 0 && argc >= 4) {
        int id = atoi(argv[2]);
        task_priority_t priority = parse_priority(argv[3]);
        if (tm_update_priority(tm, id, priority)) {
            tm_save(tm);
            printf("任务 #%d 优先级已设置为 %s\n", id, priority_names[priority]);
        } else {
            printf("任务 #%d 不存在\n", id);
        }
        
    } else if (strcmp(cmd, "due") == 0 && argc >= 4) {
        int id = atoi(argv[2]);
        time_t due_date = parse_date(argv[3]);
        if (due_date > 0 && tm_set_due_date(tm, id, due_date)) {
            tm_save(tm);
            printf("任务 #%d 截止日期已设置为 %s\n", id, argv[3]);
        } else {
            printf("设置失败，请检查日期格式 (YYYY-MM-DD)\n");
        }
        
    } else if (strcmp(cmd, "tag") == 0 && argc >= 4) {
        int id = atoi(argv[2]);
        if (tm_add_tag(tm, id, argv[3])) {
            tm_save(tm);
            printf("任务 #%d 已添加标签 #%s\n", id, argv[3]);
        } else {
            printf("添加标签失败\n");
        }
        
    } else if (strcmp(cmd, "stats") == 0) {
        print_statistics(tm);
        
    } else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
        print_help();
        
    } else {
        printf("未知命令: %s\n", cmd);
        print_help();
    }
    
    tm_free(tm);
    return 0;
}
