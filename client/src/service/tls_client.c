#include "../../include/tls_client.h"
#include <openssl/ssl.h>

SSL_CTX *tls_client_ctx_create(void) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    if (ctx == NULL)
        return NULL;

    SSL_CTX_load_verify_locations(
        ctx,
        "certs/ca.crt",
        NULL
    );

    SSL_CTX_set_verify(
        ctx,
        SSL_VERIFY_PEER,
        NULL
    );

    return ctx;
}

SSL *tls_connect(SSL_CTX *ctx, int sockfd) {
    SSL *ssl = SSL_new(ctx);

    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        return NULL;
    }

    return ssl;
}