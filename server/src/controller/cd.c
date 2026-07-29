#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"
#include "../../include/db_folder.h"
#include "../../include/db_files.h"
#include "../../include/folder_service.h"

void handle_cd(int clientfd, const char *req, session_t *session) {
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

    // Không cho phép CD .. về trước thư mục mà mình không có quyền
    if (session->in_sharing_mode == 1 && session->current_folder_id == session->share_root_folder_id && strcmp(dir, "..") == 0) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    int new_id = 0;
    if (strcmp(dir, "..") == 0) {
        ret = db_folder_find_parent(session->current_folder_id, &new_id);
    } else {
        ret = db_folder_find_folder_by_name(session->current_folder_id, dir, &new_id);
    }

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_FOLDER_DELETED || ret == DB_FOLDER_NOT_FOUND) {
        snprintf(res, sizeof(res), "422 INVALID_PATH\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_FOLDER_NAME_NOT_FOUND) {
        snprintf(res, sizeof(res), "409 NOT_A_DIRECTORY\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Oke
    session->current_folder_id = new_id;
    // Update path thu muc
    folder_service_change_dir(session, dir);

    snprintf(res, sizeof(res), "200 CHANGE_DIRECTORY_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}