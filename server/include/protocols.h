#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include "client.h"

// Xử lý logic đăng ký
void handle_register(int clientfd, const char *req);

// Xử lý logic đăng nhập
void handle_login(int clientfd, const char *req, session_t *session);

// Xử lý đăng xuất
void handle_logout(int clientfd, const char *req, session_t *session);

// Xử lý pwd
void handle_pwd(int clientfd, const char *req, session_t *session);

// Xử lý list
void handle_list(int clientfd, const char *req, session_t *session);

// Xử lý cd
void handle_cd(int clientfd, const char *req, session_t *session);

// Xử lý delete
void handle_delete(int clientfd, const char *req, session_t *session);

// Xử lý đổi tên
void handle_rename(int clientfd, const char *req, session_t *session);

// Xóa thư mục trống
void handle_rmdir(int clientfd, const char *req, session_t *session);

// Xử lý mkdir
void handle_mkdir(int clientfd, const char *req, session_t* session);

// upload
void handle_upload(int clientfd, const char *req, session_t *session);

// download
void handle_download(int clientfd, const char *req, session_t *session);

// share file
void handle_share_file(int clientfd, const char *req, session_t *session);

// share folder
void handle_share_folder(int clientfd, const char *req, session_t *session);

// unshare file
void handle_unshare_file(int clientfd, const char *req, session_t *session);

// unshare folder
void handle_unshare_folder(int clientfd, const char *req, session_t *session);

// hiển thị các user đang được chia sẻ tài nguyên của tôi
void handle_list_share_file(int clientfd, const char *req, session_t *session);

void handle_list_share_folder(int clientfd, const char *req, session_t *session);

// hiển thị các tài nguyên đang được chia sẻ với tôi
void handle_list_shared_files_with_me(int clientfd, const char *req, session_t *session);

void handle_list_shared_folders_with_me(int clientfd, const char *req, session_t *session);

// tải về files được chia sẻ với tôi
void handle_download_share(int clientfd, const char *req, session_t *session);

// Di chuyển vào thư mục được chia sẻ
void handle_open_shared_folder(int clientfd, const char *req, session_t *session);

// thoát khỏi chế độ chia sẻ
void handle_exit_shared(int clientfd, const char *req, session_t *session);

// Xử lý khi người dùng gửi giao thức không hỗ trợ
void handle_unsupported(int clientfd, const char *req, session_t *session);

#endif