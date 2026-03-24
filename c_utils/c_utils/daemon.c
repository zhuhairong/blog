#include "daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define MAX_PATH_LEN 256

static volatile sig_atomic_t g_shutdown_requested = 0;
static volatile sig_atomic_t g_reload_requested = 0;

static void default_signal_handler(int signum) {
    switch (signum) {
        case SIGTERM:
        case SIGINT:
            g_shutdown_requested = 1;
            break;
        case SIGHUP:
            g_reload_requested = 1;
            break;
    }
}

static void setup_signal_handlers(daemon_signal_handler_t handler) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    
    if (handler) {
        sa.sa_handler = handler;
    } else {
        sa.sa_handler = default_signal_handler;
    }
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}

daemon_config_t daemon_default_config(const char *name) {
    daemon_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.name = name ? name : "daemon";
    config.working_dir = "/";
    config.pid_file = NULL;
    config.log_file = NULL;
    config.error_file = NULL;
    config.umask_value = 0;
    config.close_all_fds = true;
    config.redirect_stdio = true;
    config.signal_handler = NULL;
    
    return config;
}

pid_t daemon_is_running(const char *pid_file) {
    if (!pid_file) return -1;
    
    FILE *fp = fopen(pid_file, "r");
    if (!fp) return -1;
    
    pid_t pid = -1;
    if (fscanf(fp, "%d", &pid) != 1) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    
    if (pid <= 0) return -1;
    
    if (kill(pid, 0) == 0) {
        return pid;
    }
    
    if (errno == ESRCH) {
        return -1;
    }
    
    return pid;
}

bool daemon_write_pid(const char *pid_file, pid_t pid) {
    if (!pid_file || pid <= 0) return false;
    
    int fd = open(pid_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return false;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", pid);
    
    ssize_t written = write(fd, buf, strlen(buf));
    close(fd);
    
    return written > 0;
}

bool daemon_remove_pid(const char *pid_file) {
    if (!pid_file) return false;
    return unlink(pid_file) == 0;
}

bool daemon_setup_signals(daemon_signal_handler_t handler) {
    setup_signal_handlers(handler);
    return true;
}

static bool redirect_stdio(const char *log_file, const char *error_file) {
    int null_fd = open("/dev/null", O_RDWR);
    if (null_fd < 0) return false;
    
    if (log_file) {
        int log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (log_fd < 0) {
            close(null_fd);
            return false;
        }
        dup2(log_fd, STDOUT_FILENO);
        close(log_fd);
    } else {
        dup2(null_fd, STDOUT_FILENO);
    }
    
    if (error_file) {
        int err_fd = open(error_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (err_fd < 0) {
            close(null_fd);
            return false;
        }
        dup2(err_fd, STDERR_FILENO);
        close(err_fd);
    } else {
        dup2(log_file ? STDOUT_FILENO : null_fd, STDERR_FILENO);
    }
    
    dup2(null_fd, STDIN_FILENO);
    close(null_fd);
    
    return true;
}

static void close_all_fds(void) {
    long maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd < 0) maxfd = 1024;
    
    for (long fd = 3; fd < maxfd; fd++) {
        close(fd);
    }
}

daemon_context_t* daemon_create(const daemon_config_t *config, daemon_error_t *error) {
    daemon_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        cfg = daemon_default_config(NULL);
    }
    
    if (cfg.pid_file) {
        pid_t existing_pid = daemon_is_running(cfg.pid_file);
        if (existing_pid > 0) {
            if (error) *error = DAEMON_ERROR_ALREADY_RUNNING;
            return NULL;
        }
        daemon_remove_pid(cfg.pid_file);
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        if (error) *error = DAEMON_ERROR_FORK;
        return NULL;
    }
    
    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            daemon_context_t *ctx = malloc(sizeof(daemon_context_t));
            if (ctx) {
                memset(ctx, 0, sizeof(daemon_context_t));
                ctx->pid = pid;
                ctx->running = true;
                if (cfg.pid_file) {
                    strncpy(ctx->pid_file_path, cfg.pid_file, MAX_PATH_LEN - 1);
                }
                if (cfg.log_file) {
                    strncpy(ctx->log_file_path, cfg.log_file, MAX_PATH_LEN - 1);
                }
            }
            if (error) *error = DAEMON_OK;
            return ctx;
        }
        if (error) *error = DAEMON_ERROR_FORK;
        return NULL;
    }
    
    if (setsid() < 0) {
        if (error) *error = DAEMON_ERROR_SETSID;
        exit(1);
    }
    
    pid = fork();
    if (pid < 0) {
        if (error) *error = DAEMON_ERROR_FORK;
        exit(1);
    }
    
    if (pid > 0) {
        exit(0);
    }
    
    umask(cfg.umask_value);
    
    if (cfg.working_dir) {
        if (chdir(cfg.working_dir) < 0) {
            if (error) *error = DAEMON_ERROR_CHDIR;
            exit(1);
        }
    }
    
    if (cfg.close_all_fds) {
        close_all_fds();
    }
    
    if (cfg.redirect_stdio) {
        if (!redirect_stdio(cfg.log_file, cfg.error_file)) {
            if (error) *error = DAEMON_ERROR_OPEN;
            exit(1);
        }
    }
    
    if (cfg.pid_file) {
        if (!daemon_write_pid(cfg.pid_file, getpid())) {
            if (error) *error = DAEMON_ERROR_PID_FILE;
            exit(1);
        }
    }
    
    setup_signal_handlers(cfg.signal_handler);
    
    daemon_context_t *ctx = malloc(sizeof(daemon_context_t));
    if (!ctx) {
        if (error) *error = DAEMON_ERROR_MEMORY;
        exit(1);
    }
    
    memset(ctx, 0, sizeof(daemon_context_t));
    ctx->pid = getpid();
    ctx->running = true;
    if (cfg.pid_file) {
        strncpy(ctx->pid_file_path, cfg.pid_file, MAX_PATH_LEN - 1);
    }
    if (cfg.log_file) {
        strncpy(ctx->log_file_path, cfg.log_file, MAX_PATH_LEN - 1);
    }
    
    if (error) *error = DAEMON_OK;
    return ctx;
}

void daemon_free(daemon_context_t *ctx) {
    if (!ctx) return;
    
    if (ctx->pid_file_path[0] != '\0') {
        daemon_remove_pid(ctx->pid_file_path);
    }
    
    free(ctx);
}

bool daemon_stop(const char *pid_file, int timeout_ms) {
    if (!pid_file) return false;
    
    pid_t pid = daemon_is_running(pid_file);
    if (pid <= 0) return true;
    
    if (kill(pid, SIGTERM) < 0) {
        return false;
    }
    
    if (timeout_ms < 0) {
        while (kill(pid, 0) == 0) {
            usleep(100000);
        }
        daemon_remove_pid(pid_file);
        return true;
    }
    
    int elapsed = 0;
    while (elapsed < timeout_ms) {
        if (kill(pid, 0) != 0) {
            daemon_remove_pid(pid_file);
            return true;
        }
        usleep(100000);
        elapsed += 100;
    }
    
    if (kill(pid, SIGKILL) < 0) {
        return false;
    }
    
    usleep(100000);
    daemon_remove_pid(pid_file);
    return true;
}

bool daemon_reload(const char *pid_file) {
    if (!pid_file) return false;
    
    pid_t pid = daemon_is_running(pid_file);
    if (pid <= 0) return false;
    
    return kill(pid, SIGHUP) == 0;
}

bool daemon_wait(daemon_context_t *ctx, int timeout_ms) {
    if (!ctx) return false;
    
    if (timeout_ms < 0) {
        while (ctx->running) {
            sleep(1);
        }
        return true;
    }
    
    int elapsed = 0;
    while (elapsed < timeout_ms && ctx->running) {
        usleep(100000);
        elapsed += 100;
    }
    
    return ctx->running == false;
}

const char* daemon_strerror(daemon_error_t error) {
    switch (error) {
        case DAEMON_OK: return "Success";
        case DAEMON_ERROR_FORK: return "Fork failed";
        case DAEMON_ERROR_SETSID: return "Setsid failed";
        case DAEMON_ERROR_CHDIR: return "Change directory failed";
        case DAEMON_ERROR_OPEN: return "Open file failed";
        case DAEMON_ERROR_DUP2: return "Dup2 failed";
        case DAEMON_ERROR_PID_FILE: return "PID file operation failed";
        case DAEMON_ERROR_ALREADY_RUNNING: return "Daemon already running";
        case DAEMON_ERROR_INVALID_PARAM: return "Invalid parameter";
        case DAEMON_ERROR_SIGNAL: return "Signal operation failed";
        case DAEMON_ERROR_MEMORY: return "Memory allocation failed";
        default: return "Unknown error";
    }
}
