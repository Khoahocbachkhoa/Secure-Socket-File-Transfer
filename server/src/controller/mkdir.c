#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"
#include "../../include/db_folder.h"
#include "../../include/db_files.h"
#include "../../include/folder_service.h"

// Chỉ hỗ trợ duy nhất tạo 1 dir với một lệnh
void handle_mkdir(int clientfd, const char *req, session_t* session) {
    char res[256];
    char dir[256];
    char cmd[16];
    char temp[16];

    int ret = sscanf(req, "%s %s %s\r\n", cmd, dir, temp);
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

    if (!folder_service_check_fname(dir)) {
        snprintf(res, sizeof(res), "422 INVALID_FOLDER_NAME\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    bool flag;
    ret = db_folder_check_exist_dirname(session->current_folder_id, dir, &flag);
    if (ret != OK) {
        snprintf(res, sizeof(res), "500 ERROR_SERV\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (flag == false) {
        snprintf(res, sizeof(res), "409 FOLDER_ALREADY_EXISTS\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Tạo folder mới
    ret = db_folder_create_new_folder(session->current_folder_id, session->user_id, dir);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERV\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    snprintf(res, sizeof(res), "200 MKDIR_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}