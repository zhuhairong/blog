#include "pipe.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

pipe_config_t pipe_default_config(void) {
    pipe_config_t config = {
        .type = PIPE_TYPE_ANONYMOUS,
        .name = NULL,
        .non_blocking = false,
        .buffer_size = 4096,
        .timeout_ms = 0
    };
    return config;
}

bool pipe_create(pipe_t *p) {
    return pipe_create_ex(p, NULL, NULL);
}

bool pipe_create_ex(pipe_t *p, const pipe_config_t *config, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    memset(p, 0, sizeof(pipe_t));
    p->config = config ? *config : pipe_default_config();
    
    int fds[2];
    if (pipe(fds) == -1) {
        if (error) *error = PIPE_ERROR_CREATE_FAILED;
        return false;
    }
    
    p->read_fd = fds[0];
    p->write_fd = fds[1];
    p->is_open = true;
    
    if (p->config.non_blocking) {
        pipe_set_non_blocking(p, true, NULL);
    }
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_create_named(pipe_t *p, const char *name, pipe_error_t *error) {
    if (!p || !name) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    memset(p, 0, sizeof(pipe_t));
    p->config = pipe_default_config();
    p->config.type = PIPE_TYPE_NAMED;
    p->config.name = name;
    
    if (mkfifo(name, 0666) == -1 && errno != EEXIST) {
        if (error) *error = PIPE_ERROR_CREATE_FAILED;
        return false;
    }
    
    p->read_fd = open(name, O_RDONLY | O_NONBLOCK);
    if (p->read_fd == -1) {
        if (error) *error = PIPE_ERROR_OPEN_FAILED;
        return false;
    }
    
    p->write_fd = open(name, O_WRONLY | O_NONBLOCK);
    if (p->write_fd == -1) {
        close(p->read_fd);
        if (error) *error = PIPE_ERROR_OPEN_FAILED;
        return false;
    }
    
    p->is_open = true;
    
    if (error) *error = PIPE_OK;
    return true;
}

void pipe_close(pipe_t *p) {
    pipe_close_ex(p, NULL);
}

bool pipe_close_ex(pipe_t *p, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (p->read_fd >= 0) {
        close(p->read_fd);
        p->read_fd = -1;
    }
    
    if (p->write_fd >= 0) {
        close(p->write_fd);
        p->write_fd = -1;
    }
    
    p->is_open = false;
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_read(pipe_t *p, void *buf, size_t len, size_t *bytes_read, pipe_error_t *error) {
    if (!p || !buf) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!p->is_open || p->read_fd < 0) {
        if (error) *error = PIPE_ERROR_INVALID_HANDLE;
        return false;
    }
    
    ssize_t n = read(p->read_fd, buf, len);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (bytes_read) *bytes_read = 0;
            if (error) *error = PIPE_OK;
            return true;
        }
        if (error) *error = PIPE_ERROR_READ_FAILED;
        return false;
    }
    
    if (bytes_read) *bytes_read = (size_t)n;
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_write(pipe_t *p, const void *buf, size_t len, size_t *bytes_written, pipe_error_t *error) {
    if (!p || !buf) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!p->is_open || p->write_fd < 0) {
        if (error) *error = PIPE_ERROR_INVALID_HANDLE;
        return false;
    }
    
    ssize_t n = write(p->write_fd, buf, len);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (bytes_written) *bytes_written = 0;
            if (error) *error = PIPE_OK;
            return true;
        }
        if (error) *error = PIPE_ERROR_WRITE_FAILED;
        return false;
    }
    
    if (bytes_written) *bytes_written = (size_t)n;
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_read_line(pipe_t *p, char *buf, size_t len, size_t *bytes_read, pipe_error_t *error) {
    if (!p || !buf || len == 0) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    size_t total = 0;
    char c;
    
    while (total < len - 1) {
        size_t n;
        if (!pipe_read(p, &c, 1, &n, error)) {
            return false;
        }
        
        if (n == 0) break;
        
        buf[total++] = c;
        
        if (c == '\n') break;
    }
    
    buf[total] = '\0';
    if (bytes_read) *bytes_read = total;
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_write_line(pipe_t *p, const char *buf, pipe_error_t *error) {
    if (!p || !buf) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    size_t len = strlen(buf);
    size_t written;
    
    if (!pipe_write(p, buf, len, &written, error)) {
        return false;
    }
    
    if (len > 0 && buf[len-1] != '\n') {
        if (!pipe_write(p, "\n", 1, &written, error)) {
            return false;
        }
    }
    
    return true;
}

bool pipe_is_readable(pipe_t *p, int timeout_ms, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!p->is_open || p->read_fd < 0) {
        if (error) *error = PIPE_ERROR_INVALID_HANDLE;
        return false;
    }
    
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(p->read_fd, &read_fds);
    
    struct timeval tv;
    struct timeval *ptv = NULL;
    if (timeout_ms >= 0) {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ptv = &tv;
    }
    
    int result = select(p->read_fd + 1, &read_fds, NULL, NULL, ptv);
    
    if (result < 0) {
        if (error) *error = PIPE_ERROR_READ_FAILED;
        return false;
    }
    
    if (result == 0) {
        if (error) *error = PIPE_ERROR_TIMEOUT;
        return false;
    }
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_is_writable(pipe_t *p, int timeout_ms, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!p->is_open || p->write_fd < 0) {
        if (error) *error = PIPE_ERROR_INVALID_HANDLE;
        return false;
    }
    
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(p->write_fd, &write_fds);
    
    struct timeval tv;
    struct timeval *ptv = NULL;
    if (timeout_ms >= 0) {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ptv = &tv;
    }
    
    int result = select(p->write_fd + 1, NULL, &write_fds, NULL, ptv);
    
    if (result < 0) {
        if (error) *error = PIPE_ERROR_WRITE_FAILED;
        return false;
    }
    
    if (result == 0) {
        if (error) *error = PIPE_ERROR_TIMEOUT;
        return false;
    }
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_set_non_blocking(pipe_t *p, bool non_blocking, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    int flags;
    
    if (p->read_fd >= 0) {
        flags = fcntl(p->read_fd, F_GETFL);
        if (flags < 0) {
            if (error) *error = PIPE_ERROR_INVALID_HANDLE;
            return false;
        }
        if (non_blocking) {
            flags |= O_NONBLOCK;
        } else {
            flags &= ~O_NONBLOCK;
        }
        if (fcntl(p->read_fd, F_SETFL, flags) < 0) {
            if (error) *error = PIPE_ERROR_INVALID_HANDLE;
            return false;
        }
    }
    
    if (p->write_fd >= 0) {
        flags = fcntl(p->write_fd, F_GETFL);
        if (flags < 0) {
            if (error) *error = PIPE_ERROR_INVALID_HANDLE;
            return false;
        }
        if (non_blocking) {
            flags |= O_NONBLOCK;
        } else {
            flags &= ~O_NONBLOCK;
        }
        if (fcntl(p->write_fd, F_SETFL, flags) < 0) {
            if (error) *error = PIPE_ERROR_INVALID_HANDLE;
            return false;
        }
    }
    
    p->config.non_blocking = non_blocking;
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_get_state(const pipe_t *p, bool *is_open, pipe_error_t *error) {
    if (!p) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    if (is_open) *is_open = p->is_open;
    
    if (error) *error = PIPE_OK;
    return true;
}

bool pipe_copy(pipe_t *dest, const pipe_t *src, pipe_error_t *error) {
    if (!dest || !src) {
        if (error) *error = PIPE_ERROR_NULL_PTR;
        return false;
    }
    
    memcpy(dest, src, sizeof(pipe_t));
    
    if (error) *error = PIPE_OK;
    return true;
}

const char* pipe_error_string(pipe_error_t error) {
    switch (error) {
        case PIPE_OK: return "Success";
        case PIPE_ERROR_NULL_PTR: return "Null pointer error";
        case PIPE_ERROR_CREATE_FAILED: return "Pipe creation failed";
        case PIPE_ERROR_OPEN_FAILED: return "Pipe open failed";
        case PIPE_ERROR_READ_FAILED: return "Read failed";
        case PIPE_ERROR_WRITE_FAILED: return "Write failed";
        case PIPE_ERROR_CLOSE_FAILED: return "Close failed";
        case PIPE_ERROR_INVALID_HANDLE: return "Invalid handle";
        case PIPE_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case PIPE_ERROR_TIMEOUT: return "Timeout";
        default: return "Unknown error";
    }
}
