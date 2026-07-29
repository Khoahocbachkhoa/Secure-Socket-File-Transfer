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

void handle_unshare_folder(int clientfd, const char *req, session_t *session) {
    char res[256];

    char cmd[32];
    char foldername[256];
    char username[256];

    int user_id;
    int folder_id;
    int ret;

    ret = sscanf(req, "%s %s %s\r\n", cmd, foldername, username);

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

    /* Tìm user */
    ret = db_user_find_id_by_username(username, &user_id);

    if (ret == DB_USER_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 USER_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    /* Tìm folder */
    ret = db_folder_find_folder_by_name(
            session->current_folder_id,
            foldername,
            &folder_id);

    if (ret == DB_FOLDER_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FOLDER_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    /* Thu hồi quyền */
    ret = db_sharing_revoke_folder_access(folder_id, user_id);

    if (ret == DB_SHARING_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 SHARE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);

        return;
    }

    snprintf(res, sizeof(res), "200 UNSHARE_FOLDER_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}