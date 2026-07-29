#include "../../include/tls_server.h"
#include <openssl/ssl.h>

SSL_CTX *tls_server_ctx_create(void) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

    if (ctx == NULL)
        return NULL;

    SSL_CTX_use_certificate_file(
        ctx,
        "certs/server.crt",
        SSL_FILETYPE_PEM
    );

    SSL_CTX_use_PrivateKey_file(
        ctx,
        "certs/server.key",
        SSL_FILETYPE_PEM
    );

    return ctx;
}

SSL *tls_accept(SSL_CTX *ctx, int sockfd) {
    SSL *ssl = SSL_new(ctx);

    SSL_set_fd(ssl, sockfd);

    if (SSL_accept(ssl) <= 0) {
        SSL_free(ssl);
        return NULL;
    }

    return ssl;
}