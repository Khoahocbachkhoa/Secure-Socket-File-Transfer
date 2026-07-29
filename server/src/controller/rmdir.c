#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/database.h"
#include "../../include/file_service.h"
#include "../../include/db_files.h"
#include "../../include/db_folder.h"

void handle_rmdir(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char dirname[256];
    char res[256];

    int folder_id;
    int ret;

    ret = sscanf(req, "%s %s\r\n", cmd, dirname);

    if (ret != 2) {
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

    ret = db_folder_find_folder_by_name(session->current_folder_id, dirname, &folder_id);

    if (ret == DB_FOLDER_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FOLDER_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    if (ret != OK) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    /* kiểm tra rỗng */
    ret = db_folder_is_empty(folder_id);

    if (ret != OK) {

        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    if (ret == DB_FOLDER_NOT_EMPTY) {
        snprintf(res, sizeof(res), "400 FOLDER_NOT_EMPTY\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    ret = db_folder_soft_delete(folder_id);

    if (ret != OK) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    snprintf(res, sizeof(res), "200 RMDIR_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}