#ifndef TLS_SERVER_H
#define TLS_SERVER_H

#include <openssl/ssl.h>

SSL_CTX *tls_server_ctx_create(void);

SSL *tls_accept(SSL_CTX *ctx, int sockfd);

#endif