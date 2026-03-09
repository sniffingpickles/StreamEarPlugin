#ifndef STREAMEAR_OUTPUT_H
#define STREAMEAR_OUTPUT_H

#include <obs-module.h>
#include "ring-buffer.h"
#include "tcp-sender.h"

struct streamear_output {
    obs_output_t *output;
    ring_buffer_t ring_buffer;
    tcp_sender_t tcp_sender;
    uint16_t port;
    bool active;
};

extern struct obs_output_info streamear_output_info;

#endif
