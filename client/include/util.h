#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

// check xem tên folder có hợp lệ không
bool check_folder_name(const char *fname);

// check xem tên file có hợp lệ không
bool check_file_name(const char *fname);

// Chuẩn hóa 1 path
void normalize_path(const char *path, char *buf, int buf_size);

// Lấy tên file của 1 path
void get_file_name(const char *path, char *buf, int buf_size);

#endif