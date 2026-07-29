#include <unistd.h>
#include <sys/socket.h>

#include "../../include/transport.h"
#include "../../include/client.h"

// ssize_t net_send(int sockfd, const void* buf, size_t len, int flag) {
//     return send(sockfd, buf, len, flag);
// }

// ssize_t net_recv(int sockfd, void *buf, size_t len, int flag) {
//     return recv(sockfd, buf, len, flag);
// }

// Su dung TLS
ssize_t net_send(int sockfd, const void *buf, size_t len, int flag) {
    SSL *ssl = g_ssl_table[sockfd];

    if (ssl)
        return SSL_write(ssl, buf, len);

    return send(sockfd, buf, len, flag);
}

ssize_t net_recv(int sockfd, void *buf, size_t len, int flag) {
    SSL *ssl = g_ssl_table[sockfd];

    if (ssl)
        return SSL_read(ssl, buf, len);

    return recv(sockfd, buf, len, flag);
}

ssize_t net_recv_all(int fd, void *buf, size_t len, int flag) {
    size_t total = 0;
    char *p = (char *)buf;

    while (total < len) {
        ssize_t n = net_recv(fd, p + total, len - total, flag);

        // Client disconnect
        if (n == 0) {
            break;
        }

        // Error
        if (n < 0) {
            return -1;
        }

        total += n;
    }

    return (ssize_t)total;
}

ssize_t net_send_all(int fd, void *buf, size_t len, int flag) {
    size_t total = 0;
    char *p = (char *)buf;

    while (total < len) {
        ssize_t n = net_send(fd, p + total, len - total, flag);

        // Error
        if (n <= 0) {
            return -1;
        }

        total += n;
    }

    return (ssize_t)total;
}