/**
 * 守护进程库演示程序
 *
 * 功能：
 * - 基本守护进程创建
 * - PID 文件管理
 * - 信号处理
 * - 日志输出
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "../c_utils/daemon.h"

static volatile sig_atomic_t g_running = 1;
static volatile sig_atomic_t g_reload = 0;

static void signal_handler(int signum) {
    switch (signum) {
        case SIGTERM:
        case SIGINT:
            g_running = 0;
            break;
        case SIGHUP:
            g_reload = 1;
            break;
        case SIGUSR1:
            printf("Received SIGUSR1\n");
            break;
        case SIGUSR2:
            printf("Received SIGUSR2\n");
            break;
    }
}

static void write_log(FILE *log, const char *message) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log, "[%s] %s\n", time_buf, message);
    fflush(log);
}

// 演示 1: 基本守护进程创建
static void demo_basic_daemon(void) {
    printf("\n=== 演示 1: 基本守护进程创建 ===\n");
    printf("注意: 此演示会创建一个后台守护进程\n");
    printf("守护进程将运行 5 秒后自动退出\n\n");
    
    const char *pid_file = "/tmp/demo_daemon_basic.pid";
    const char *log_file = "/tmp/demo_daemon_basic.log";
    
    // 检查是否已在运行
    pid_t existing = daemon_is_running(pid_file);
    if (existing > 0) {
        printf("守护进程已在运行 (PID: %d)\n", existing);
        printf("停止现有守护进程...\n");
        daemon_stop(pid_file, 5000);
    }
    
    daemon_config_t config = daemon_default_config("demo_basic");
    config.pid_file = pid_file;
    config.log_file = log_file;
    config.signal_handler = signal_handler;
    
    daemon_error_t error;
    daemon_context_t *ctx = daemon_create(&config, &error);
    
    if (!ctx) {
        printf("创建守护进程失败: %s\n", daemon_strerror(error));
        return;
    }
    
    // 在守护进程中运行
    FILE *log = fopen(log_file, "a");
    if (log) {
        write_log(log, "守护进程启动");
        write_log(log, "守护进程 PID: %d");
        fprintf(log, "守护进程 PID: %d\n", ctx->pid);
        
        int counter = 0;
        while (g_running && counter < 5) {
            sleep(1);
            counter++;
            write_log(log, "守护进程运行中...");
            
            if (g_reload) {
                write_log(log, "收到重新加载信号");
                g_reload = 0;
            }
        }
        
        write_log(log, "守护进程退出");
        fclose(log);
    }
    
    daemon_free(ctx);
    printf("守护进程已退出\n");
    printf("日志文件: %s\n", log_file);
}

// 演示 2: 检查守护进程状态
static void demo_check_status(void) {
    printf("\n=== 演示 2: 检查守护进程状态 ===\n");
    
    const char *pid_file = "/tmp/demo_daemon_basic.pid";
    
    pid_t pid = daemon_is_running(pid_file);
    if (pid > 0) {
        printf("守护进程正在运行 (PID: %d)\n", pid);
    } else {
        printf("守护进程未运行\n");
    }
}

// 演示 3: 停止守护进程
static void demo_stop_daemon(void) {
    printf("\n=== 演示 3: 停止守护进程 ===\n");
    
    const char *pid_file = "/tmp/demo_daemon_basic.pid";
    
    pid_t pid = daemon_is_running(pid_file);
    if (pid > 0) {
        printf("正在停止守护进程 (PID: %d)...\n", pid);
        
        if (daemon_stop(pid_file, 5000)) {
            printf("守护进程已停止\n");
        } else {
            printf("停止守护进程失败\n");
        }
    } else {
        printf("守护进程未运行\n");
    }
}

// 演示 4: 重新加载配置
static void demo_reload_daemon(void) {
    printf("\n=== 演示 4: 重新加载配置 ===\n");
    
    const char *pid_file = "/tmp/demo_daemon_basic.pid";
    
    if (daemon_reload(pid_file)) {
        printf("已发送重新加载信号\n");
    } else {
        printf("发送重新加载信号失败\n");
    }
}

// 演示 5: PID 文件管理
static void demo_pid_file_management(void) {
    printf("\n=== 演示 5: PID 文件管理 ===\n");
    
    const char *pid_file = "/tmp/demo_pid_test.pid";
    
    // 写入 PID 文件
    pid_t my_pid = getpid();
    printf("当前进程 PID: %d\n", my_pid);
    
    if (daemon_write_pid(pid_file, my_pid)) {
        printf("PID 文件已创建: %s\n", pid_file);
        
        // 检查是否运行
        pid_t check_pid = daemon_is_running(pid_file);
        printf("检查结果: PID %d %s\n", 
               check_pid, 
               (check_pid == my_pid) ? "匹配" : "不匹配");
        
        // 删除 PID 文件
        if (daemon_remove_pid(pid_file)) {
            printf("PID 文件已删除\n");
        }
    } else {
        printf("创建 PID 文件失败\n");
    }
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");
    
    printf("错误码说明:\n");
    printf("  DAEMON_OK: %s\n", daemon_strerror(DAEMON_OK));
    printf("  DAEMON_ERROR_FORK: %s\n", daemon_strerror(DAEMON_ERROR_FORK));
    printf("  DAEMON_ERROR_SETSID: %s\n", daemon_strerror(DAEMON_ERROR_SETSID));
    printf("  DAEMON_ERROR_CHDIR: %s\n", daemon_strerror(DAEMON_ERROR_CHDIR));
    printf("  DAEMON_ERROR_PID_FILE: %s\n", daemon_strerror(DAEMON_ERROR_PID_FILE));
    printf("  DAEMON_ERROR_ALREADY_RUNNING: %s\n", daemon_strerror(DAEMON_ERROR_ALREADY_RUNNING));
}

// 演示 7: 配置选项
static void demo_config_options(void) {
    printf("\n=== 演示 7: 配置选项 ===\n");
    
    daemon_config_t config = daemon_default_config("my_daemon");
    
    printf("默认配置:\n");
    printf("  名称: %s\n", config.name);
    printf("  工作目录: %s\n", config.working_dir);
    printf("  PID 文件: %s\n", config.pid_file ? config.pid_file : "(无)");
    printf("  日志文件: %s\n", config.log_file ? config.log_file : "(无)");
    printf("  文件掩码: %03o\n", config.umask_value);
    printf("  关闭所有 FD: %s\n", config.close_all_fds ? "是" : "否");
    printf("  重定向标准 IO: %s\n", config.redirect_stdio ? "是" : "否");
    
    printf("\n自定义配置示例:\n");
    config.working_dir = "/var/run";
    config.pid_file = "/var/run/my_daemon.pid";
    config.log_file = "/var/log/my_daemon.log";
    config.umask_value = 022;
    config.close_all_fds = false;
    
    printf("  工作目录: %s\n", config.working_dir);
    printf("  PID 文件: %s\n", config.pid_file);
    printf("  日志文件: %s\n", config.log_file);
    printf("  文件掩码: %03o\n", config.umask_value);
    printf("  关闭所有 FD: %s\n", config.close_all_fds ? "是" : "否");
}

// 演示 8: 信号处理
static void demo_signal_handling(void) {
    printf("\n=== 演示 8: 信号处理 ===\n");
    
    printf("支持的信号:\n");
    printf("  SIGTERM - 优雅退出\n");
    printf("  SIGINT  - 优雅退出 (Ctrl+C)\n");
    printf("  SIGHUP  - 重新加载配置\n");
    printf("  SIGUSR1 - 自定义信号 1\n");
    printf("  SIGUSR2 - 自定义信号 2\n");
    printf("  SIGPIPE - 忽略\n");
    printf("  SIGTSTP - 忽略\n");
    printf("  SIGTTIN - 忽略\n");
    printf("  SIGTTOU - 忽略\n");
    
    printf("\n设置信号处理器...\n");
    if (daemon_setup_signals(signal_handler)) {
        printf("信号处理器已设置\n");
    }
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("    守护进程库演示程序\n");
    printf("========================================\n");
    
    if (argc > 1) {
        if (strcmp(argv[1], "start") == 0) {
            demo_basic_daemon();
        } else if (strcmp(argv[1], "stop") == 0) {
            demo_stop_daemon();
        } else if (strcmp(argv[1], "status") == 0) {
            demo_check_status();
        } else if (strcmp(argv[1], "reload") == 0) {
            demo_reload_daemon();
        } else {
            printf("用法: %s [start|stop|status|reload]\n", argv[0]);
            return 1;
        }
        return 0;
    }
    
    // 运行所有演示
    demo_config_options();
    demo_error_handling();
    demo_signal_handling();
    demo_pid_file_management();
    demo_check_status();
    
    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");
    
    printf("\n提示: 使用以下命令测试守护进程:\n");
    printf("  %s start   - 启动守护进程\n", argv[0]);
    printf("  %s status  - 检查状态\n", argv[0]);
    printf("  %s reload  - 重新加载配置\n", argv[0]);
    printf("  %s stop    - 停止守护进程\n", argv[0]);
    
    return 0;
}
