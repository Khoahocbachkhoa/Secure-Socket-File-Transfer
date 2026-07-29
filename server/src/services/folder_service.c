#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/folder_service.h"

// Cập nhật path mới
void folder_service_change_dir(session_t *session, const char *dir) {
    if (strcmp(dir, "..") == 0) {
        // Nếu ở root thì giữ nguyên
        if (strcmp(session->cwd, "/") != 0) {
            char *last = strrchr( session->cwd, '/');

            if (last != NULL) {
                if (last == session->cwd) {
                    // quay về root
                    session->cwd[1] = '\0';
                } else {
                    // Bỏ đi path cuối
                    *last = '\0';
                }
            }
        }
    } else {
        // Hiện tại đang ở root
        if (strcmp(session->cwd, "/") == 0) {
            strncat(
                session->cwd,
                dir,
                sizeof(session->cwd) - strlen(session->cwd) - 1
            );
        } else {
            // Thêm / + new_path
            strncat(session->cwd, "/", sizeof(session->cwd) - strlen(session->cwd) - 1);

            strncat(session->cwd, dir, sizeof(session->cwd) - strlen(session->cwd) - 1);
        }
    }
}

// Kiểm tra tính hợp lệ của một folder name
bool folder_service_check_fname(char *name) {
    if (name == NULL || *name == '\0')
        return false;

    // chặn "." và ".."
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return false;

    for (char *p = name; *p; ++p) {
        // không cho separator
        if (*p == '/' || *p == '\\')
            return false;

        // không cho ký tự control
        if (iscntrl((unsigned char)*p))
            return false;
    }

    return true;
}
