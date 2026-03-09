#ifndef STREAMEAR_RING_BUFFER_H
#define STREAMEAR_RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE 256
#define MAX_FRAME_SIZE 8192

typedef struct {
    uint8_t data[MAX_FRAME_SIZE];
    size_t length;
    int64_t pts;
} ring_entry_t;

typedef struct {
    ring_entry_t entries[RING_BUFFER_SIZE];
    volatile uint32_t write_pos;
    volatile uint32_t read_pos;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb);
bool ring_buffer_push(ring_buffer_t *rb, const uint8_t *data, size_t length, int64_t pts);
bool ring_buffer_pop(ring_buffer_t *rb, ring_entry_t *out);
bool ring_buffer_empty(const ring_buffer_t *rb);

#endif
