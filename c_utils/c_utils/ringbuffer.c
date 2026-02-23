#include "ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct ringbuffer_s {
    unsigned char *buffer;
    size_t size;
    size_t head;
    size_t tail;
    size_t count;
};

ringbuffer_t* ringbuffer_create(size_t size) {
    ringbuffer_t *rb = malloc(sizeof(ringbuffer_t));
    rb->buffer = malloc(size);
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    return rb;
}

void ringbuffer_free(ringbuffer_t *rb) {
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

size_t ringbuffer_available_read(const ringbuffer_t *rb) { return rb->count; }
size_t ringbuffer_available_write(const ringbuffer_t *rb) { return rb->size - rb->count; }
bool ringbuffer_is_full(const ringbuffer_t *rb) { return rb->count == rb->size; }
bool ringbuffer_is_empty(const ringbuffer_t *rb) { return rb->count == 0; }

size_t ringbuffer_write(ringbuffer_t *rb, const void *data, size_t len) {
    size_t can_write = ringbuffer_available_write(rb);
    size_t to_write = len < can_write ? len : can_write;
    
    for (size_t i = 0; i < to_write; i++) {
        rb->buffer[rb->head] = ((unsigned char*)data)[i];
        rb->head = (rb->head + 1) % rb->size;
    }
    rb->count += to_write;
    return to_write;
}

size_t ringbuffer_read(ringbuffer_t *rb, void *data, size_t len) {
    size_t can_read = ringbuffer_available_read(rb);
    size_t to_read = len < can_read ? len : can_read;
    
    for (size_t i = 0; i < to_read; i++) {
        ((unsigned char*)data)[i] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
    }
    rb->count -= to_read;
    return to_read;
}
