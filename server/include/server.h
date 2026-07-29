#ifndef SERVER_H
#define SERVER_H

// Tạo luồng xử lý user
void* client_thread(void *arg);

// Khởi tạo server
void start_server(int port);

#endif