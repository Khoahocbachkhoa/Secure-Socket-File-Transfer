#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"
#include "../../include/db_folder.h"
#include "../../include/db_user.h"
#include "../../include/db_files.h"
#include "../../include/db_sharing.h"

void handle_unshare_file(int clientfd, const char *req, session_t *session) {
    char res[256];
    char cmd[16];
    char filename[256];
    char username[256];
    int user_id, file_id;
    int ret;

    ret = sscanf(req, "%s %s %s\r\n", cmd, filename, username);
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
    if (session->in_sharing_mode == 1) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    // Không được hủy quyền chia sẻ file nếu không phải chủ sỡ hữu

    // Tìm user có id là username
    ret = db_user_find_id_by_username(username, &user_id);
    if (ret == DB_USER_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 USER_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        //printf("db_user_find_id_userName eroror\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Tìm file có id là filename
    ret = db_file_find_id_by_name(session->current_folder_id, filename, &file_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        //printf("file find id by name eroror\n");
        return;
    } else if (ret == DB_FILE_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FILE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Hủy quyền chia sẻ file
    ret = db_sharing_revoke_file_access(file_id, user_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        //printf("grant access eroror\n");
        return;
    } else if (ret == DB_SHARING_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 SHARE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    snprintf(res, sizeof(res), "200 OKE\r\n");
    net_send(clientfd, res, strlen(res), 0);
}