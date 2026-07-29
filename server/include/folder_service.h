#ifndef FOLDER_SERVICE_H
#define FOLDER_SERVICE_H

#include "client.h"
#include <stdbool.h>

// Cập nhật path mới
void folder_service_change_dir(session_t *session, const char *dir);

// Kiểm tra tính hợp lệ của một tên folder
bool folder_service_check_fname(char *name);

#endif