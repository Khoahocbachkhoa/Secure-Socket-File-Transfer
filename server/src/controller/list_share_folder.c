#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/db_files.h"
#include "../../include/db_sharing.h"
#include "../../include/transport.h"

void handle_list_share_folder(int clientfd, const char *req, session_t *session) {
    char res[256];
    char cmd[32];
    char foldername[256];

    int folder_id;
    int ret;

    ret = sscanf(req, "%31s %255s", cmd, foldername);

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

    if (session->in_sharing_mode == 1) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    /* Tìm folder */
    ret = db_folder_find_folder_by_name(
            session->current_folder_id,
            foldername,
            &folder_id);

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (ret == DB_FOLDER_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FOLDER_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    /* Lấy danh sách user được share */
    shared_user_t users[256];
    int n = 256;

    ret = db_sharing_list_users_shared_folder(
            folder_id,
            users,
            &n);

    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }
    
    /* Bắt đầu gửi danh sách */
    snprintf(res, sizeof(res), "200 LIST_SHARED_USERS_BEGIN\r\n");
    net_send(clientfd, res, strlen(res), 0);

    char buf[2048];

    for (int i = 0; i < n; ++i) {
        snprintf(buf, sizeof(buf), "%s %s\r\n", 
                users[i].username, users[i].role);

        net_send(clientfd, buf, strlen(buf), 0);
    }

    snprintf(res, sizeof(res), "200 LIST_SHARED_USERS_END\r\n");
    net_send(clientfd, res, strlen(res), 0);
}