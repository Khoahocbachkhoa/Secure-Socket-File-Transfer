#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/database.h"
#include "../../include/file_service.h"
#include "../../include/db_files.h"

void handle_rename(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char old_name[256];
    char new_name[256];
    char res[256];

    int ret;

    ret = sscanf(req, "%s %s %s\r\n", cmd, old_name, new_name);

    if (ret != 3) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "401 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Kiểm tra quyền
    if (session->in_sharing_mode == 1 && session->role == ROLE_VIEWER) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    int file_id;

    ret = db_file_find_id_by_name(session->current_folder_id, old_name, &file_id);

    if (ret == OK) {
        // Gọi service đổi tên file
        ret = db_file_rename(file_id, new_name);

        if (ret != OK) {
            snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
            net_send(clientfd, res, strlen(res), 0);
            return;
        }

        snprintf(res, sizeof(res), "200 RENAME_SUCCESS\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    int folder_id;

    ret = db_folder_find_folder_by_name(
            session->current_folder_id,
            old_name,
            &folder_id);

    if (ret == OK) {
        // Gọi service đổi tên folder
        ret = db_folder_rename(folder_id, new_name);

        if (ret != OK) {
            snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
            net_send(clientfd, res, strlen(res), 0);

            return;
        }

        snprintf(res, sizeof(res), "200 RENAME_SUCCESS\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    snprintf(res, sizeof(res), "404 RESOURCE_NOT_FOUND\r\n");
    net_send(clientfd, res, strlen(res), 0);
}