#include "log.h"
#include <string.h>
#include <time.h>

#define MAX_CALLBACKS 32

typedef struct {
    FILE *fp;
    log_level_t level;
} log_callback_t;

static struct {
    log_level_t level;
    int quiet;
    log_callback_t callbacks[MAX_CALLBACKS];
    int callback_count;
} L;

static const char *level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

void log_set_level(log_level_t level) { L.level = level; }
void log_set_quiet(int enable) { L.quiet = enable; }

int log_add_fp(FILE *fp, log_level_t level) {
    if (L.callback_count >= MAX_CALLBACKS) return -1;
    L.callbacks[L.callback_count++] = (log_callback_t){fp, level};
    return 0;
}

void log_log(log_level_t level, const char *file, int line, const char *fmt, ...) {
    if (level < L.level) return;

    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    // 终端输出
    if (!L.quiet) {
        va_list ap;
        char time_buf[16];
        strftime(time_buf, sizeof(time_buf), "%H:%M:%S", lt);
        printf("%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", 
               time_buf, level_colors[level], level_names[level], file, line);
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
        printf("\n");
        fflush(stdout);
    }

    // 文件输出
    for (int i = 0; i < L.callback_count; i++) {
        log_callback_t *cb = &L.callbacks[i];
        if (level >= cb->level) {
            va_list ap;
            char time_buf[64];
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", lt);
            fprintf(cb->fp, "%s %-5s %s:%d: ", time_buf, level_names[level], file, line);
            va_start(ap, fmt);
            vfprintf(cb->fp, fmt, ap);
            va_end(ap);
            fprintf(cb->fp, "\n");
            fflush(cb->fp);
        }
    }
}
