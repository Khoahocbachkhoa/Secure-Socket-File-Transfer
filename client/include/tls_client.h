#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <openssl/ssl.h>

SSL_CTX *tls_client_ctx_create(void);

SSL *tls_connect(SSL_CTX *ctx, int sockfd);

#endif