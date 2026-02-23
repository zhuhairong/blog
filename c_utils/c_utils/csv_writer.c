#include "csv_writer.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

csv_writer_config_t csv_writer_default_config(void) {
    csv_writer_config_t config = {
        .delimiter = ',',
        .quote = '"',
        .escape = '"',
        .quote_all_fields = false,
        .trim_whitespace = false,
        .buffer_size = 4096
    };
    return config;
}

csv_writer_t* csv_writer_create(const char *filepath, const char *mode, const csv_writer_config_t *config, csv_writer_error_t *error) {
    if (!filepath || !mode) {
        if (error) *error = CSV_WRITER_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    FILE *file = fopen(filepath, mode);
    if (!file) {
        if (error) *error = CSV_WRITER_ERROR_FILE_OPEN;
        return NULL;
    }
    
    csv_writer_t *writer = csv_writer_create_from_file(file, config, error);
    if (!writer) {
        fclose(file);
        return NULL;
    }
    
    return writer;
}

csv_writer_t* csv_writer_create_from_file(FILE *file, const csv_writer_config_t *config, csv_writer_error_t *error) {
    if (!file) {
        if (error) *error = CSV_WRITER_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    csv_writer_t *writer = (csv_writer_t*)malloc(sizeof(csv_writer_t));
    if (!writer) {
        if (error) *error = CSV_WRITER_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    writer->file = file;
    writer->config = config ? *config : csv_writer_default_config();
    writer->buffer_size = writer->config.buffer_size;
    writer->buffer = (char*)malloc(writer->buffer_size);
    
    if (!writer->buffer) {
        free(writer);
        if (error) *error = CSV_WRITER_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    writer->buffer_pos = 0;
    writer->has_error = false;
    writer->error = CSV_WRITER_OK;
    writer->error_msg[0] = '\0';
    
    if (error) *error = CSV_WRITER_OK;
    return writer;
}

void csv_writer_free(csv_writer_t *writer) {
    if (!writer) return;
    
    if (writer->buffer) {
        csv_writer_flush(writer);
        free(writer->buffer);
    }
    
    if (writer->file) {
        fclose(writer->file);
    }
    
    free(writer);
}

static bool needs_quoting(const csv_writer_t *writer, const char *field) {
    if (writer->config.quote_all_fields) return true;
    
    for (const char *p = field; *p; p++) {
        if (*p == writer->config.delimiter || 
            *p == writer->config.quote || 
            *p == '\n' || *p == '\r') {
            return true;
        }
    }
    return false;
}

static bool buffer_write(csv_writer_t *writer, const char *data, size_t len) {
    if (writer->buffer_pos + len >= writer->buffer_size) {
        if (!csv_writer_flush(writer)) {
            return false;
        }
    }
    
    if (len >= writer->buffer_size) {
        if (fwrite(data, 1, len, writer->file) != len) {
            writer->has_error = true;
            writer->error = CSV_WRITER_ERROR_FILE_WRITE;
            return false;
        }
        return true;
    }
    
    memcpy(writer->buffer + writer->buffer_pos, data, len);
    writer->buffer_pos += len;
    return true;
}

bool csv_writer_write_field(csv_writer_t *writer, const char *field, bool is_last) {
    if (!writer || !field) {
        if (writer) {
            writer->has_error = true;
            writer->error = CSV_WRITER_ERROR_INVALID_PARAM;
        }
        return false;
    }
    
    bool quote = needs_quoting(writer, field);
    char delim[2] = {writer->config.delimiter, '\0'};
    
    if (quote) {
        if (!buffer_write(writer, &writer->config.quote, 1)) return false;
    }
    
    for (const char *p = field; *p; p++) {
        if (*p == writer->config.quote) {
            if (!buffer_write(writer, &writer->config.escape, 1)) return false;
        }
        if (!buffer_write(writer, p, 1)) return false;
    }
    
    if (quote) {
        if (!buffer_write(writer, &writer->config.quote, 1)) return false;
    }
    
    if (is_last) {
        if (!buffer_write(writer, "\n", 1)) return false;
    } else {
        if (!buffer_write(writer, delim, 1)) return false;
    }
    
    return true;
}

bool csv_writer_write_row(csv_writer_t *writer, const char **fields, size_t count) {
    if (!writer || !fields) {
        if (writer) {
            writer->has_error = true;
            writer->error = CSV_WRITER_ERROR_INVALID_PARAM;
        }
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!csv_writer_write_field(writer, fields[i] ? fields[i] : "", i == count - 1)) {
            return false;
        }
    }
    
    return true;
}

bool csv_writer_write_row_va(csv_writer_t *writer, ...) {
    if (!writer) return false;
    
    va_list args;
    va_start(args, writer);
    
    const char *field;
    const char *fields[256];
    size_t count = 0;
    
    while ((field = va_arg(args, const char*)) != NULL) {
        if (count < 256) {
            fields[count++] = field;
        }
    }
    
    va_end(args);
    
    return csv_writer_write_row(writer, fields, count);
}

bool csv_writer_flush(csv_writer_t *writer) {
    if (!writer || !writer->buffer || writer->buffer_pos == 0) {
        return true;
    }
    
    if (fwrite(writer->buffer, 1, writer->buffer_pos, writer->file) != writer->buffer_pos) {
        writer->has_error = true;
        writer->error = CSV_WRITER_ERROR_FILE_WRITE;
        return false;
    }
    
    writer->buffer_pos = 0;
    return true;
}

bool csv_writer_has_error(const csv_writer_t *writer, csv_writer_error_t *error, const char **error_msg) {
    if (!writer) return true;
    
    if (error) *error = writer->error;
    if (error_msg) *error_msg = writer->error_msg;
    
    return writer->has_error;
}

const char* csv_writer_strerror(csv_writer_error_t error) {
    switch (error) {
        case CSV_WRITER_OK: return "Success";
        case CSV_WRITER_ERROR_FILE_OPEN: return "Failed to open file";
        case CSV_WRITER_ERROR_FILE_WRITE: return "Failed to write to file";
        case CSV_WRITER_ERROR_INVALID_PARAM: return "Invalid parameter";
        case CSV_WRITER_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case CSV_WRITER_ERROR_BUFFER_FULL: return "Buffer full";
        default: return "Unknown error";
    }
}

void csv_write_field(FILE *f, const char *field, bool last) {
    if (!f || !field) return;
    
    bool quote = strpbrk(field, ",\"\n\r") != NULL;
    if (quote) fputc('"', f);
    for (const char *p = field; *p; p++) {
        if (*p == '"') fputc('"', f);
        fputc(*p, f);
    }
    if (quote) fputc('"', f);
    fputc(last ? '\n' : ',', f);
}
