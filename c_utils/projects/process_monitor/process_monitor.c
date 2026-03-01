#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <dirent.h>
#include "process.h"
#include "cpu_usage.h"
#include "timer.h"
#include "terminal.h"
#include "argparse.h"
#include "log.h"
#include "hashmap.h"
#include "sort.h"
#include "time_utils.h"

#define MAX_PROCESSES 1000
#define REFRESH_INTERVAL 1000
#define MAX_CMDLINE_LEN 256

typedef struct {
    pid_t pid;
    pid_t ppid;
    char name[64];
    char cmdline[MAX_CMDLINE_LEN];
    char state;
    double cpu_percent;
    double mem_percent;
    unsigned long virt_mem;
    unsigned long res_mem;
    time_t start_time;
} process_info_t;

typedef struct {
    process_info_t *processes;
    size_t count;
    size_t capacity;
    double total_cpu;
    double total_mem;
    int running_count;
    int sleeping_count;
    int stopped_count;
    int zombie_count;
} process_list_t;

static volatile bool running = true;

static void signal_handler(int sig) {
    (void)sig;
    running = false;
}

static void clear_screen(void) {
    printf("\033[2J\033[H");
}

static void hide_cursor(void) {
    printf("\033[?25l");
}

static void show_cursor(void) {
    printf("\033[?25h");
}

static bool read_process_info(pid_t pid, process_info_t *info) {
    char path[256];
    FILE *fp;
    
    memset(info, 0, sizeof(process_info_t));
    info->pid = pid;
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fp = fopen(path, "r");
    if (!fp) return false;
    
    int dummy;
    char state;
    pid_t ppid;
    unsigned long utime, stime, vsize, rss;
    unsigned long long starttime;
    
    if (fscanf(fp, "%d %63s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
               &dummy, info->name, &state, &ppid,
               &dummy, &dummy, &dummy, &dummy, &dummy,
               &dummy, &dummy, &dummy, &dummy,
               &utime, &stime, &dummy, &dummy,
               &dummy, &dummy, &dummy, &dummy, &dummy, &starttime) != 23) {
        fclose(fp);
        return false;
    }
    fclose(fp);
    
    info->ppid = ppid;
    info->state = state;
    info->virt_mem = vsize;
    info->res_mem = rss * 4096;
    
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    fp = fopen(path, "r");
    if (fp) {
        size_t len = fread(info->cmdline, 1, MAX_CMDLINE_LEN - 1, fp);
        fclose(fp);
        for (size_t i = 0; i < len; i++) {
            if (info->cmdline[i] == '\0') info->cmdline[i] = ' ';
        }
        info->cmdline[len] = '\0';
    }
    
    return true;
}

static process_list_t* get_process_list(void) {
    process_list_t *list = malloc(sizeof(process_list_t));
    if (!list) return NULL;
    
    list->processes = malloc(sizeof(process_info_t) * MAX_PROCESSES);
    if (!list->processes) {
        free(list);
        return NULL;
    }
    
    list->count = 0;
    list->capacity = MAX_PROCESSES;
    list->total_cpu = 0;
    list->total_mem = 0;
    list->running_count = 0;
    list->sleeping_count = 0;
    list->stopped_count = 0;
    list->zombie_count = 0;
    
    FILE *fp = fopen("/proc", "r");
    if (!fp) {
        free(list->processes);
        free(list);
        return NULL;
    }
    
    struct dirent *entry;
    DIR *dir = opendir("/proc");
    if (!dir) {
        free(list->processes);
        free(list);
        return NULL;
    }
    
    while ((entry = readdir(dir)) != NULL && list->count < list->capacity) {
        if (entry->d_type != DT_DIR) continue;
        
        char *endptr;
        pid_t pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') continue;
        
        process_info_t info;
        if (read_process_info(pid, &info)) {
            list->processes[list->count++] = info;
            
            switch (info.state) {
                case 'R': list->running_count++; break;
                case 'S': list->sleeping_count++; break;
                case 'D': list->sleeping_count++; break;
                case 'T': list->stopped_count++; break;
                case 'Z': list->zombie_count++; break;
            }
        }
    }
    
    closedir(dir);
    return list;
}

static void free_process_list(process_list_t *list) {
    if (list) {
        free(list->processes);
        free(list);
    }
}

static void print_header(void) {
    printf("\033[7m");
    printf("%-8s %-8s %-6s %-10s %-8s %-8s %-12s %-12s %s\n",
           "PID", "PPID", "STATE", "USER", "CPU%%", "MEM%%", "VIRT(KB)", "RES(KB)", "COMMAND");
    printf("\033[0m");
}

static void print_process(const process_info_t *proc, bool highlight) {
    if (highlight) {
        printf("\033[1;33m");
    }
    
    const char *state_str;
    switch (proc->state) {
        case 'R': state_str = "RUN"; break;
        case 'S': state_str = "SLE"; break;
        case 'D': state_str = "DSK"; break;
        case 'T': state_str = "STP"; break;
        case 'Z': state_str = "ZOM"; break;
        default: state_str = "???"; break;
    }
    
    printf("%-8d %-8d %-6s %-10s %-7.1f %-7.1f %-12lu %-12lu %s\n",
           proc->pid, proc->ppid, state_str, "user",
           proc->cpu_percent, proc->mem_percent,
           proc->virt_mem / 1024, proc->res_mem / 1024,
           proc->cmdline[0] ? proc->cmdline : proc->name);
    
    if (highlight) {
        printf("\033[0m");
    }
}

static void print_system_info(const cpu_usage_stats_t *cpu_stats) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    printf("\033[1;36m");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        进程监控工具 - %s                          ║\n", time_str);
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ CPU使用率: %5.1f%%  |  用户: %5.1f%%  |  系统: %5.1f%%  |  空闲: %5.1f%%        ║\n",
           cpu_stats->total, cpu_stats->user, cpu_stats->system, cpu_stats->idle);
    printf("║ 进程总数: %-5zu  |  运行: %-4d  |  睡眠: %-4d  |  停止: %-4d  |  僵尸: %-4d   ║\n",
           0, 0, 0, 0, 0);
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\033[0m\n");
}

static void print_help(void) {
    printf("\n进程监控工具 - 帮助\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("按键说明:\n");
    printf("  q       退出程序\n");
    printf("  h       显示帮助\n");
    printf("  r       刷新列表\n");
    printf("  k       杀死进程 (输入PID)\n");
    printf("  s       按CPU排序\n");
    printf("  m       按内存排序\n");
    printf("  p       按PID排序\n");
    printf("  ↑/↓     上下移动选择\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

static int compare_by_cpu(const void *a, const void *b) {
    const process_info_t *pa = (const process_info_t *)a;
    const process_info_t *pb = (const process_info_t *)b;
    if (pb->cpu_percent > pa->cpu_percent) return 1;
    if (pb->cpu_percent < pa->cpu_percent) return -1;
    return 0;
}

static int compare_by_mem(const void *a, const void *b) {
    const process_info_t *pa = (const process_info_t *)a;
    const process_info_t *pb = (const process_info_t *)b;
    if (pb->res_mem > pa->res_mem) return 1;
    if (pb->res_mem < pa->res_mem) return -1;
    return 0;
}

static int compare_by_pid(const void *a, const void *b) {
    const process_info_t *pa = (const process_info_t *)a;
    const process_info_t *pb = (const process_info_t *)b;
    return pa->pid - pb->pid;
}

static void kill_process(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("已发送终止信号到进程 %d\n", pid);
    } else {
        printf("无法终止进程 %d: 权限不足或进程不存在\n", pid);
    }
}

static void monitor_loop(int interval_ms) {
    int sort_mode = 0;
    int selected = 0;
    char input[32];
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    hide_cursor();
    
    while (running) {
        clear_screen();
        
        cpu_usage_stats_t cpu_stats = {0};
        cpu_usage_get_overall(&cpu_stats, NULL);
        
        process_list_t *list = get_process_list();
        if (!list) {
            printf("无法获取进程列表\n");
            break;
        }
        
        switch (sort_mode) {
            case 0: qsort(list->processes, list->count, sizeof(process_info_t), compare_by_cpu); break;
            case 1: qsort(list->processes, list->count, sizeof(process_info_t), compare_by_mem); break;
            case 2: qsort(list->processes, list->count, sizeof(process_info_t), compare_by_pid); break;
        }
        
        print_system_info(&cpu_stats);
        print_header();
        
        size_t display_count = list->count > 20 ? 20 : list->count;
        for (size_t i = 0; i < display_count; i++) {
            print_process(&list->processes[i], (int)i == selected);
        }
        
        printf("\n\033[2m按 h 查看帮助, q 退出\033[0m\n");
        
        fflush(stdout);
        
        fd_set readfds;
        struct timeval tv;
        tv.tv_sec = interval_ms / 1000;
        tv.tv_usec = (interval_ms % 1000) * 1000;
        
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            char c = getchar();
            
            switch (c) {
                case 'q':
                    running = false;
                    break;
                case 'h':
                    clear_screen();
                    print_help();
                    printf("按任意键继续...");
                    getchar();
                    break;
                case 'r':
                    break;
                case 's':
                    sort_mode = 0;
                    break;
                case 'm':
                    sort_mode = 1;
                    break;
                case 'p':
                    sort_mode = 2;
                    break;
                case 'k':
                    printf("输入要终止的PID: ");
                    fflush(stdout);
                    if (fgets(input, sizeof(input), stdin)) {
                        pid_t pid = atoi(input);
                        if (pid > 0) {
                            kill_process(pid);
                        }
                    }
                    printf("按任意键继续...");
                    getchar();
                    break;
                case '\033':
                    getchar();
                    c = getchar();
                    if (c == 'A' && selected > 0) selected--;
                    else if (c == 'B' && selected < (int)display_count - 1) selected++;
                    break;
            }
        }
        
        free_process_list(list);
    }
    
    show_cursor();
    clear_screen();
    printf("进程监控工具已退出\n");
}

static void list_processes_once(void) {
    cpu_usage_stats_t cpu_stats = {0};
    cpu_usage_get_overall(&cpu_stats, NULL);
    
    process_list_t *list = get_process_list();
    if (!list) {
        printf("无法获取进程列表\n");
        return;
    }
    
    qsort(list->processes, list->count, sizeof(process_info_t), compare_by_cpu);
    
    print_system_info(&cpu_stats);
    print_header();
    
    size_t display_count = list->count > 30 ? 30 : list->count;
    for (size_t i = 0; i < display_count; i++) {
        print_process(&list->processes[i], false);
    }
    
    printf("\n总计: %zu 个进程\n", list->count);
    
    free_process_list(list);
}

static void print_usage(const char *program) {
    printf("用法: %s [选项]\n\n", program);
    printf("选项:\n");
    printf("  -h, --help        显示帮助信息\n");
    printf("  -l, --list        列出进程并退出\n");
    printf("  -i, --interval N  设置刷新间隔(毫秒, 默认1000)\n");
    printf("  -p, --pid N       显示指定进程信息\n");
    printf("  -k, --kill N      终止指定进程\n");
    printf("\n示例:\n");
    printf("  %s              # 启动交互式监控\n", program);
    printf("  %s -l           # 列出所有进程\n", program);
    printf("  %s -p 1234      # 显示PID为1234的进程信息\n", program);
    printf("  %s -k 1234      # 终止PID为1234的进程\n", program);
}

int main(int argc, char **argv) {
    int interval_ms = REFRESH_INTERVAL;
    bool list_mode = false;
    pid_t target_pid = 0;
    bool kill_mode = false;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            list_mode = true;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interval") == 0) {
            if (i + 1 < argc) {
                interval_ms = atoi(argv[++i]);
                if (interval_ms < 100) interval_ms = 100;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pid") == 0) {
            if (i + 1 < argc) {
                target_pid = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--kill") == 0) {
            if (i + 1 < argc) {
                target_pid = atoi(argv[++i]);
                kill_mode = true;
            }
        }
    }
    
    if (kill_mode && target_pid > 0) {
        kill_process(target_pid);
        return 0;
    }
    
    if (target_pid > 0) {
        process_info_t info;
        if (read_process_info(target_pid, &info)) {
            printf("进程信息:\n");
            printf("  PID:     %d\n", info.pid);
            printf("  PPID:    %d\n", info.ppid);
            printf("  状态:    %c\n", info.state);
            printf("  名称:    %s\n", info.name);
            printf("  命令:    %s\n", info.cmdline);
            printf("  虚拟内存: %lu KB\n", info.virt_mem / 1024);
            printf("  物理内存: %lu KB\n", info.res_mem / 1024);
        } else {
            printf("无法获取进程 %d 的信息\n", target_pid);
        }
        return 0;
    }
    
    if (list_mode) {
        list_processes_once();
        return 0;
    }
    
    printf("启动进程监控工具...\n");
    printf("按 q 退出, h 查看帮助\n\n");
    sleep(1);
    
    monitor_loop(interval_ms);
    
    return 0;
}
