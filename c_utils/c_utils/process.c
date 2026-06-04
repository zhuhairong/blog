#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

// 默认配置
process_config_t process_default_config(void) {
    process_config_t config = {
        .capture_stdout = true,
        .capture_stderr = false,
        .merge_stderr = false,
        .timeout_ms = 0,
        .max_output_size = 0,
        .working_dir = NULL,
        .envp = NULL
    };
    return config;
}

char* process_exec(const char *cmd, int *exit_code) {
    if (!cmd) return NULL;
    
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    char *output = NULL;
    size_t size = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        char *new_output = realloc(output, size + len + 1);
        if (!new_output) {
            free(output);
            pclose(fp);
            return NULL;
        }
        output = new_output;
        memcpy(output + size, buffer, len);
        size += len;
        output[size] = '\0';
    }

    int status = pclose(fp);
    if (exit_code) *exit_code = WEXITSTATUS(status);
    
    if (output) return output;
    char *empty = strdup("");
    return empty ? empty : NULL;  // strdup 失败返回 NULL
}

bool process_exec_ex(const char *cmd, const process_config_t *config, process_result_t *result, process_error_t *error) {
    if (!cmd || !result) {
        if (error) *error = PROCESS_ERROR_NULL_PTR;
        return false;
    }
    
    memset(result, 0, sizeof(process_result_t));
    
    int pipe_stdout[2] = {-1, -1};
    int pipe_stderr[2] = {-1, -1};
    
    if (pipe(pipe_stdout) == -1) {
        if (error) *error = PROCESS_ERROR_PIPE_FAILED;
        return false;
    }
    
    if (!config || config->capture_stderr) {
        if (pipe(pipe_stderr) == -1) {
            close(pipe_stdout[0]);
            close(pipe_stdout[1]);
            if (error) *error = PROCESS_ERROR_PIPE_FAILED;
            return false;
        }
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(pipe_stdout[0]);
        close(pipe_stdout[1]);
        if (pipe_stderr[0] != -1) close(pipe_stderr[0]);
        if (pipe_stderr[1] != -1) close(pipe_stderr[1]);
        if (error) *error = PROCESS_ERROR_FORK_FAILED;
        return false;
    }
    
    if (pid == 0) {
        // 子进程
        close(pipe_stdout[0]);
        dup2(pipe_stdout[1], STDOUT_FILENO);
        close(pipe_stdout[1]);
        
        if (pipe_stderr[1] != -1) {
            close(pipe_stderr[0]);
            dup2(pipe_stderr[1], STDERR_FILENO);
            close(pipe_stderr[1]);
        }
        
        if (config && config->working_dir) {
            chdir(config->working_dir);
        }
        
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(127);
    }
    
    // 父进程
    close(pipe_stdout[1]);
    if (pipe_stderr[1] != -1) close(pipe_stderr[1]);

    int timeout_ms = (config && config->timeout_ms > 0) ? config->timeout_ms : 0;
    struct timeval deadline, now;
    struct timeval *p_deadline = NULL;
    if (timeout_ms > 0) {
        gettimeofday(&deadline, NULL);
        deadline.tv_sec += timeout_ms / 1000;
        deadline.tv_usec += (timeout_ms % 1000) * 1000;
        if (deadline.tv_usec >= 1000000) {
            deadline.tv_sec++;
            deadline.tv_usec -= 1000000;
        }
        p_deadline = &deadline;
    }

    char buffer[4096];
    ssize_t n;
    int status;
    bool child_exited = false;
    bool timed_out = false;

    while (!child_exited) {
        // 构建 select 的 fd_set
        fd_set readfds;
        FD_ZERO(&readfds);
        int max_fd = -1;

        if (pipe_stdout[0] != -1) {
            FD_SET(pipe_stdout[0], &readfds);
            if (pipe_stdout[0] > max_fd) max_fd = pipe_stdout[0];
        }
        if (pipe_stderr[0] != -1) {
            FD_SET(pipe_stderr[0], &readfds);
            if (pipe_stderr[0] > max_fd) max_fd = pipe_stderr[0];
        }

        // 计算 select 超时时间
        struct timeval tv;
        struct timeval *p_tv = NULL;
        if (p_deadline) {
            gettimeofday(&now, NULL);
            if (now.tv_sec > deadline.tv_sec ||
                (now.tv_sec == deadline.tv_sec && now.tv_usec >= deadline.tv_usec)) {
                // 超时
                timed_out = true;
                break;
            }
            tv.tv_sec = deadline.tv_sec - now.tv_sec;
            tv.tv_usec = deadline.tv_usec - now.tv_usec;
            if (tv.tv_usec < 0) {
                tv.tv_sec--;
                tv.tv_usec += 1000000;
            }
            if (tv.tv_sec < 0) {
                timed_out = true;
                break;
            }
            p_tv = &tv;
        }

        if (max_fd >= 0) {
            int sel_ret = select(max_fd + 1, &readfds, NULL, NULL, p_tv);
            if (sel_ret < 0) {
                if (errno == EINTR) continue;
                break;
            }
        } else {
            // 没有可读的 fd，直接等待子进程
            if (p_tv) {
                usleep(p_tv->tv_sec * 1000000 + p_tv->tv_usec);
            }
        }

        // 读取 stdout
        if (pipe_stdout[0] != -1 && FD_ISSET(pipe_stdout[0], &readfds)) {
            n = read(pipe_stdout[0], buffer, sizeof(buffer));
            if (n > 0) {
                char *new_stdout = realloc(result->stdout, result->stdout_len + n + 1);
                if (new_stdout) {
                    result->stdout = new_stdout;
                    memcpy(result->stdout + result->stdout_len, buffer, n);
                    result->stdout_len += n;
                    result->stdout[result->stdout_len] = '\0';
                }
            } else if (n == 0) {
                close(pipe_stdout[0]);
                pipe_stdout[0] = -1;
            }
        }

        // 读取 stderr
        if (pipe_stderr[0] != -1 && FD_ISSET(pipe_stderr[0], &readfds)) {
            n = read(pipe_stderr[0], buffer, sizeof(buffer));
            if (n > 0) {
                char *new_stderr = realloc(result->stderr, result->stderr_len + n + 1);
                if (new_stderr) {
                    result->stderr = new_stderr;
                    memcpy(result->stderr + result->stderr_len, buffer, n);
                    result->stderr_len += n;
                    result->stderr[result->stderr_len] = '\0';
                }
            } else if (n == 0) {
                close(pipe_stderr[0]);
                pipe_stderr[0] = -1;
            }
        }

        // 检查子进程是否退出
        pid_t wret = waitpid(pid, &status, WNOHANG);
        if (wret > 0) {
            child_exited = true;
        } else if (wret < 0) {
            break;
        }
    }

    // 关闭残留的管道 fd
    if (pipe_stdout[0] != -1) close(pipe_stdout[0]);
    if (pipe_stderr[0] != -1) close(pipe_stderr[0]);

    if (timed_out) {
        // 超时：杀死子进程
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);  // 回收僵尸进程
        result->exit_code = -1;
        result->succeeded = false;
        result->timed_out = true;
        if (error) *error = PROCESS_ERROR_TIMEOUT;
        return false;
    }

    result->exit_code = WEXITSTATUS(status);
    result->succeeded = (result->exit_code == 0);
    result->timed_out = false;

    if (error) *error = PROCESS_OK;
    return result->succeeded;
}

int process_run(const char *cmd) {
    if (!cmd) return -1;
    return system(cmd);
}

int process_run_ex(const char *cmd, const process_config_t *config, process_error_t *error) {
    process_result_t result;
    if (process_exec_ex(cmd, config, &result, error)) {
        process_free_result(&result);
        return 0;
    }
    return result.exit_code;
}

bool process_exec_argv(char *const argv[], const process_config_t *config, process_result_t *result, process_error_t *error) {
    if (!argv || !argv[0] || !result) {
        if (error) *error = PROCESS_ERROR_NULL_PTR;
        return false;
    }
    
    // 构建命令字符串
    size_t cmd_len = 0;
    for (int i = 0; argv[i]; i++) {
        cmd_len += strlen(argv[i]) + 3; // 空格和引号
    }
    
    char *cmd = malloc(cmd_len + 1);
    if (!cmd) {
        if (error) *error = PROCESS_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    cmd[0] = '\0';
    size_t pos = 0;
    for (int i = 0; argv[i]; i++) {
        if (i > 0) pos += snprintf(cmd + pos, cmd_len + 1 - pos, " ");
        pos += snprintf(cmd + pos, cmd_len + 1 - pos, "%s", argv[i]);
    }
    
    bool ret = process_exec_ex(cmd, config, result, error);
    free(cmd);
    return ret;
}

void process_free_result(process_result_t *result) {
    if (!result) return;
    free(result->stdout);
    free(result->stderr);
    result->stdout = NULL;
    result->stderr = NULL;
    result->stdout_len = 0;
    result->stderr_len = 0;
}

bool process_command_exists(const char *cmd) {
    if (!cmd) return false;
    
    char *path = getenv("PATH");
    if (!path) return false;
    
    char *path_copy = strdup(path);
    if (!path_copy) return false;
    
    char *dir = strtok(path_copy, ":");
    while (dir) {
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return true;
        }
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return false;
}

char* process_which(const char *cmd, process_error_t *error) {
    if (!cmd) {
        if (error) *error = PROCESS_ERROR_NULL_PTR;
        return NULL;
    }
    
    char *path = getenv("PATH");
    if (!path) {
        if (error) *error = PROCESS_ERROR_EXEC_FAILED;
        return NULL;
    }
    
    char *path_copy = strdup(path);
    if (!path_copy) {
        if (error) *error = PROCESS_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    char *dir = strtok(path_copy, ":");
    while (dir) {
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            char *result = strdup(full_path);
            if (!result) {
                if (error) *error = PROCESS_ERROR_MEMORY_ALLOC;
                return NULL;
            }
            if (error) *error = PROCESS_OK;
            return result;
        }
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    if (error) *error = PROCESS_ERROR_EXEC_FAILED;
    return NULL;
}

pid_t process_get_pid(void) {
    return getpid();
}

pid_t process_get_ppid(void) {
    return getppid();
}

char* process_get_cwd(process_error_t *error) {
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        if (error) *error = PROCESS_ERROR_EXEC_FAILED;
        return NULL;
    }
    if (error) *error = PROCESS_OK;
    return cwd;
}

bool process_set_cwd(const char *path, process_error_t *error) {
    if (!path) {
        if (error) *error = PROCESS_ERROR_NULL_PTR;
        return false;
    }
    
    if (chdir(path) != 0) {
        if (error) *error = PROCESS_ERROR_EXEC_FAILED;
        return false;
    }
    
    if (error) *error = PROCESS_OK;
    return true;
}

const char* process_error_string(process_error_t error) {
    switch (error) {
        case PROCESS_OK: return "Success";
        case PROCESS_ERROR_NULL_PTR: return "Null pointer";
        case PROCESS_ERROR_FORK_FAILED: return "Fork failed";
        case PROCESS_ERROR_PIPE_FAILED: return "Pipe failed";
        case PROCESS_ERROR_EXEC_FAILED: return "Execution failed";
        case PROCESS_ERROR_WAIT_FAILED: return "Wait failed";
        case PROCESS_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case PROCESS_ERROR_TIMEOUT: return "Timeout";
        default: return "Unknown error";
    }
}
