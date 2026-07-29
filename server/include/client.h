#ifndef CLIENT_H
#define CLIENT_H

#define MAX_CLIENT 2048

#include "tls_server.h"
typedef enum {
    ROLE_EDITOR,
    ROLE_VIEWER
} share_role_t;

typedef struct {
    int clientfd;
    SSL *ssl;
} ClientContext;

extern SSL *g_ssl_table[MAX_CLIENT];

typedef struct {
    int logged_in;
    int user_id;
    char username[64];
    
    long current_folder_id; // Id thu muc hien tai
    char cwd[256]; // Thu muc lam viec hien tai
    char old_cwd[256]; // for restore while exit sharing mode

    int in_sharing_mode; // = 1 neu dang o trong folder duoc chia se voi toi

    int shared_owner_id;
    char shared_owner[64];

    share_role_t role;

    long share_root_folder_id; // Thư mục gốc của phân quyền
    long old_folder_id; // Thu muc truoc khi kia khi dang khong o trong sharing mode
    char share_cwd[256]; // Thu muc hien tai khi dang o trong che do chia se

    SSL *ssl;
} session_t;

// Xử lý request của client
void handle_client(int clientfd, SSL *ssl);

#endif