#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/db_files.h"
#include "../../include/db_sharing.h"
#include "../../include/transport.h"

// Hien thi cac user dang duoc ban cap quyen truy cap file nay
void handle_list_share_file(int clientfd, const char *req, session_t *session) {
    char res[256];
    char cmd[32];
    char filename[256];
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

    // Khong co quyen neu khong phai chu so huu
    if (session->in_sharing_mode == 1) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Tim file boi id
    ret = db_file_find_id_by_name(session->current_folder_id, filename, &file_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_FILE_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FILE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Gui cac user dang duoc chia se quyen
    char *users[256];
    int n = 256;
    ret = db_sharing_list_users_shared(file_id, users, &n);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    snprintf(res, sizeof(res), "200 LIST_FILES_SHARED_BEGIN\r\n");
    net_send(clientfd, res, strlen(res), 0);

    char buf[256];
    for (int i = 0; i < n; ++i) {
        snprintf(buf, sizeof(buf), "%s\n", users[i]);
        free(users[i]);
        net_send(clientfd, buf, strlen(buf), 0);
    }

    snprintf(res, sizeof(res), "200 LIST_FILES_SHARED_END\r\n");
    net_send(clientfd, res, strlen(res), 0);
}