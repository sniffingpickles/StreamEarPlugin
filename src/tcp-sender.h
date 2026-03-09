#ifndef STREAMEAR_TCP_SENDER_H
#define STREAMEAR_TCP_SENDER_H

#include "ring-buffer.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#define INVALID_SOCK INVALID_SOCKET
#else
typedef int socket_t;
#define INVALID_SOCK -1
#endif

typedef struct {
    ring_buffer_t *ring_buffer;
    uint16_t port;
    volatile bool running;
    volatile bool connected;
    socket_t sock;
    void *thread; /* pthread_t or HANDLE */
} tcp_sender_t;

bool tcp_sender_start(tcp_sender_t *sender, ring_buffer_t *rb, uint16_t port);
void tcp_sender_stop(tcp_sender_t *sender);
bool tcp_sender_is_connected(const tcp_sender_t *sender);

#endif
