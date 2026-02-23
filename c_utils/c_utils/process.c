#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
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
    
    return output ? output : strdup("");
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
    
    // 读取输出
    char buffer[4096];
    ssize_t n;
    
    while ((n = read(pipe_stdout[0], buffer, sizeof(buffer))) > 0) {
        char *new_stdout = realloc(result->stdout, result->stdout_len + n + 1);
        if (!new_stdout) {
            close(pipe_stdout[0]);
            if (pipe_stderr[0] != -1) close(pipe_stderr[0]);
            if (error) *error = PROCESS_ERROR_MEMORY_ALLOC;
            return false;
        }
        result->stdout = new_stdout;
        memcpy(result->stdout + result->stdout_len, buffer, n);
        result->stdout_len += n;
        result->stdout[result->stdout_len] = '\0';
    }
    
    close(pipe_stdout[0]);
    
    if (pipe_stderr[0] != -1) {
        while ((n = read(pipe_stderr[0], buffer, sizeof(buffer))) > 0) {
            char *new_stderr = realloc(result->stderr, result->stderr_len + n + 1);
            if (!new_stderr) {
                close(pipe_stderr[0]);
                if (error) *error = PROCESS_ERROR_MEMORY_ALLOC;
                return false;
            }
            result->stderr = new_stderr;
            memcpy(result->stderr + result->stderr_len, buffer, n);
            result->stderr_len += n;
            result->stderr[result->stderr_len] = '\0';
        }
        close(pipe_stderr[0]);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
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
    for (int i = 0; argv[i]; i++) {
        if (i > 0) strcat(cmd, " ");
        strcat(cmd, argv[i]);
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
            if (error) *error = PROCESS_OK;
            return strdup(full_path);
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
