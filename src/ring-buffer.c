#include "ring-buffer.h"
#include <string.h>

void ring_buffer_init(ring_buffer_t *rb)
{
    memset(rb, 0, sizeof(ring_buffer_t));
    rb->write_pos = 0;
    rb->read_pos = 0;
}

bool ring_buffer_push(ring_buffer_t *rb, const uint8_t *data, size_t length, int64_t pts)
{
    if (length > MAX_FRAME_SIZE)
        return false;

    uint32_t pos = rb->write_pos % RING_BUFFER_SIZE;
    ring_entry_t *entry = &rb->entries[pos];

    memcpy(entry->data, data, length);
    entry->length = length;
    entry->pts = pts;

    /* Memory barrier: ensure data is written before advancing position */
    __sync_synchronize();
    rb->write_pos++;

    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, ring_entry_t *out)
{
    if (rb->read_pos == rb->write_pos)
        return false;

    uint32_t pos = rb->read_pos % RING_BUFFER_SIZE;
    ring_entry_t *entry = &rb->entries[pos];

    memcpy(out, entry, sizeof(ring_entry_t));

    __sync_synchronize();
    rb->read_pos++;

    return true;
}

bool ring_buffer_empty(const ring_buffer_t *rb)
{
    return rb->read_pos == rb->write_pos;
}
