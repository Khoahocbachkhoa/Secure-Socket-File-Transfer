#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/database.h"
#include "../../include/file_service.h"
#include "../../include/db_files.h"

void handle_delete(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char filename[256];
    char res[256];

    int file_id;
    int ret;

    ret = sscanf(req, "%s %s\r\n", cmd, filename);

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

    /* Tìm file trong thư mục hiện tại */
    ret = db_file_find_id_by_name(session->current_folder_id, filename, &file_id);

    if (ret == DB_FILE_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FILE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd,  res,  strlen(res), 0);
        return;
    }

    ret = db_file_soft_delete(file_id);

    if (ret != OK) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    snprintf(res, sizeof(res), "200 DELETE_SUCCESS\r\n");

    net_send(clientfd, res, strlen(res), 0);
}