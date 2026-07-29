#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <unistd.h>

// Wrap send và recv để mã hóa kênh truyền sau này
ssize_t net_send(int sockfd, const void* buf, size_t len, int flag);

ssize_t net_recv(int sockfd, void* buf,size_t len, int flag);

// Do send và recv có thể ko nhận đủ byte, cần có hàm bảo đàm nhận đủ
ssize_t net_recv_all(int fd, void *buf, size_t len, int flag);

ssize_t net_send_all(int fd, void *buf, size_t len, int flag);

#endif