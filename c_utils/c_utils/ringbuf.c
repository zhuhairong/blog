#include "ringbuf.h"
#include <stdlib.h>
#include <string.h>

struct ringbuf_s {
    uint8_t *buffer;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t size;
};

ringbuf_t* ringbuf_create(size_t capacity) {
    ringbuf_t *rb = malloc(sizeof(ringbuf_t));
    if (!rb) return NULL;
    rb->buffer = malloc(capacity);
    if (!rb->buffer) {
        free(rb);
        return NULL;
    }
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->size = 0;
    return rb;
}

void ringbuf_free(ringbuf_t *rb) {
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

size_t ringbuf_size(const ringbuf_t *rb) {
    return rb ? rb->size : 0;
}

size_t ringbuf_avail(const ringbuf_t *rb) {
    return rb ? (rb->capacity - rb->size) : 0;
}

bool ringbuf_is_full(const ringbuf_t *rb) {
    return rb && rb->size == rb->capacity;
}

bool ringbuf_is_empty(const ringbuf_t *rb) {
    return !rb || rb->size == 0;
}

size_t ringbuf_write(ringbuf_t *rb, const uint8_t *data, size_t len) {
    if (!rb || len == 0) return 0;
    size_t can_write = rb->capacity - rb->size;
    if (len > can_write) len = can_write;

    size_t first_part = rb->capacity - rb->tail;
    if (len <= first_part) {
        memcpy(rb->buffer + rb->tail, data, len);
    } else {
        memcpy(rb->buffer + rb->tail, data, first_part);
        memcpy(rb->buffer, data + first_part, len - first_part);
    }

    rb->tail = (rb->tail + len) % rb->capacity;
    rb->size += len;
    return len;
}

size_t ringbuf_read(ringbuf_t *rb, uint8_t *data, size_t len) {
    if (!rb || len == 0 || rb->size == 0) return 0;
    if (len > rb->size) len = rb->size;

    size_t first_part = rb->capacity - rb->head;
    if (len <= first_part) {
        if (data) memcpy(data, rb->buffer + rb->head, len);
    } else {
        if (data) {
            memcpy(data, rb->buffer + rb->head, first_part);
            memcpy(data + first_part, rb->buffer, len - first_part);
        }
    }

    rb->head = (rb->head + len) % rb->capacity;
    rb->size -= len;
    return len;
}
