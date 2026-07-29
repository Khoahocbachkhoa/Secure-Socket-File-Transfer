#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include "db_files.h"

// Truy cập disk và lưu một file
bool storage_save_file(const char *path, int clientfd, size_t filesize);

// Gửi một file từ disk
bool storage_send_file(const char *paht, int clientfd, size_t filesize);

// Xóa 1 file trên disk
// bool storage_delete_file(const char *path);

#endif