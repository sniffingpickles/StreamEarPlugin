#include "tcp-sender.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#define closesocket close
#endif

static socket_t tcp_connect(uint16_t port)
{
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCK)
        return INVALID_SOCK;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        closesocket(sock);
        return INVALID_SOCK;
    }

    return sock;
}

static bool tcp_send_frame(socket_t sock, const ring_entry_t *entry)
{
    /* Frame format: 4 bytes length + N bytes AAC data + 8 bytes PTS */
    uint32_t frame_len = htonl((uint32_t)entry->length);
    int64_t pts_net = entry->pts;

    /* Send frame length */
    if (send(sock, (const char *)&frame_len, 4, 0) != 4)
        return false;

    /* Send AAC frame data */
    size_t sent = 0;
    while (sent < entry->length) {
        int n = send(sock, (const char *)(entry->data + sent),
                     (int)(entry->length - sent), 0);
        if (n <= 0)
            return false;
        sent += n;
    }

    /* Send PTS */
    if (send(sock, (const char *)&pts_net, 8, 0) != 8)
        return false;

    return true;
}

static void *sender_thread_func(void *arg)
{
    tcp_sender_t *sender = (tcp_sender_t *)arg;
    ring_entry_t entry;

    while (sender->running) {
        /* Try to connect if not connected */
        if (sender->sock == INVALID_SOCK) {
            sender->connected = false;
            sender->sock = tcp_connect(sender->port);
            if (sender->sock == INVALID_SOCK) {
                /* Wait before retry */
#ifdef _WIN32
                Sleep(1000);
#else
                usleep(1000000);
#endif
                continue;
            }
            sender->connected = true;
        }

        /* Try to pop and send a frame */
        if (ring_buffer_pop(sender->ring_buffer, &entry)) {
            if (!tcp_send_frame(sender->sock, &entry)) {
                closesocket(sender->sock);
                sender->sock = INVALID_SOCK;
                sender->connected = false;
            }
        } else {
            /* No frames, sleep briefly */
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
        }
    }

    if (sender->sock != INVALID_SOCK) {
        closesocket(sender->sock);
        sender->sock = INVALID_SOCK;
    }
    sender->connected = false;

    return NULL;
}

bool tcp_sender_start(tcp_sender_t *sender, ring_buffer_t *rb, uint16_t port)
{
    sender->ring_buffer = rb;
    sender->port = port;
    sender->running = true;
    sender->connected = false;
    sender->sock = INVALID_SOCK;

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    sender->thread = CreateThread(NULL, 0,
        (LPTHREAD_START_ROUTINE)sender_thread_func, sender, 0, NULL);
    return sender->thread != NULL;
#else
    pthread_t *t = malloc(sizeof(pthread_t));
    if (!t) return false;
    sender->thread = t;
    return pthread_create(t, NULL, sender_thread_func, sender) == 0;
#endif
}

void tcp_sender_stop(tcp_sender_t *sender)
{
    sender->running = false;

#ifdef _WIN32
    if (sender->thread) {
        WaitForSingleObject(sender->thread, 5000);
        CloseHandle(sender->thread);
    }
    WSACleanup();
#else
    if (sender->thread) {
        pthread_join(*(pthread_t *)sender->thread, NULL);
        free(sender->thread);
    }
#endif

    sender->thread = NULL;
}

bool tcp_sender_is_connected(const tcp_sender_t *sender)
{
    return sender->connected;
}
