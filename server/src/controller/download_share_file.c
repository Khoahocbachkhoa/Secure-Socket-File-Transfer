#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/db_permission.h"
#include "../../include/file_service.h"

void handle_download_share(int clientfd, const char *req, session_t *session) {
    char res[256];
    char cmd[32];
    int file_id;
    int ret;

    ret = sscanf(req, "%s %d\r\n", cmd, &file_id);
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

    // Kiểm tra xem có quyền tải file không (được chia sẻ không)
    ret = db_permission_check_access_file(session->user_id, file_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_PERMISSION_DENIED) {
        snprintf(res, sizeof(res), "403 ACCESS_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Bắt đầu tải file về
    ret = file_service_download_by_file_id(clientfd, file_id, session);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } 

    // Gửi thành công
    snprintf(res, sizeof(res), "200 DOWNLOAD_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}