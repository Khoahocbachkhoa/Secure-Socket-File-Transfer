#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdio.h>
#include <unistd.h>

// Wrap send và recv để mã hóa kênh truyền sau này
ssize_t net_send(int sockfd, const void* buf, size_t len, int flag);

ssize_t net_recv(int sockfd, void* buf,size_t len, int flag);

// Đảm bảo gửi đủ dữ liệu
ssize_t net_recv_all(int fd, void *buf, size_t len, int flag);

ssize_t net_send_all(int fd, void *buf, size_t len, int flag);

// Gửi command
int send_command(int sockfd, const char *cmd);

// Nhận phản hồi từ server
int recv_response(int sockfd);

// Nhận phản hồi từ server (1 dòng)
int recv_line(int sockfd, char *buf, size_t size);

// Nhận phản hồi từ server (có thể có nhiều dòng) cho tới khi gặp marker
int recv_multiline_reponse(int sockfd, const char *marker);

// Nhận phản hồi từ server và ghi vào buf
int recv_response_to_buf(int sockfd, char *buf, int buf_size);

#endif