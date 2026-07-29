#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

#include "../../include/transport.h"
#include "../../include/tls_client.h"
#include "../../include/state.h"
#include <openssl/ssl.h>

// todo : sau này thêm chức năng mã hóa kênh truyền
// ssize_t net_send(int sockfd, const void* buf, size_t len, int flag) {
//     return send(sockfd, buf, len, flag);
// }

// ssize_t net_recv(int sockfd, void *buf, size_t len, int flag) {
//     return recv(sockfd, buf, len, flag);
// }

// Thêm TLS
ssize_t net_send(int sockfd, const void* buf, size_t len, int flag) {
    return SSL_write(state->ssl, buf, len);
}

ssize_t net_recv(int sockfd, void *buf, size_t len, int flag) {
    return SSL_read(state->ssl, buf, len);
}

int send_command(int sockfd, const char *cmd) {
    net_send(sockfd, cmd, strlen(cmd), 0);
    return 0;
}

int recv_response(int sockfd) {
    char buf[8192];

    // Đọc phản hồi từ server và in kết quả
    int n = net_recv(sockfd, buf, sizeof(buf)-1, 0);
        
    if (n <= 0) {
        return -1;
    }

    buf[n] = '\0';
    printf("%s", buf);
    return 0;
}

// Đọc phản hồi vào buf
int recv_response_to_buf(int sockfd, char *buf, int buf_size) {
    if (buf == NULL || buf_size <= 1)
        return -1;

    // int n = net_recv(sockfd, buf, buf_size - 1, 0);
    int n = recv_line(sockfd, buf, buf_size - 1);

    if (n <= 0)
        return -1;

    buf[n] = '\0';

    return n;
}

// Đọc phản hồi 1 dòng
int recv_line(int sockfd, char *buf, size_t size) {
    if (buf == NULL || size == 0)
        return -1;

    size_t pos = 0;

    while (pos < size - 1) {
        char c;
        int n = net_recv(sockfd, &c, 1, 0);

        if (n <= 0)
            return -1;

        buf[pos++] = c;

        if (c == '\n')
            break;
    }

    buf[pos] = '\0';

    return (int)pos;
}

// Đọc và in liên tục cho tới khi gặp marker
int recv_multiline_reponse(int sockfd, const char *marker) {
    if (marker == NULL)
        return -1;

    char line[1024];

    while (1) {
        int n = recv_line(sockfd, line, sizeof(line));

        if (n <= 0)
            return -1;

        /*
         * "200 LIST_END"
         * "200 LIST_SHARED_USERS_END"
         */

        if (strncmp(line, marker, strlen(marker)) == 0)
            break;

        printf("%s", line);
    }

    return 0;
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