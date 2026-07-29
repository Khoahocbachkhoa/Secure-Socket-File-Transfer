#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <openssl/ssl.h>

typedef struct {
    bool is_logged_in;
    char username[64];
    char cwd[64];
    int sockfd;

    SSL_CTX *ssl_ctx; // Lưu ngữ cảnh SSL
    SSL *ssl; // Lưu kết nối SSL
} ClientState;

extern ClientState *state;

void init_state(ClientState *state, int fd);

#endif