#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/client.h"
#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/database.h"
#include "../../include/file_service.h"

void handle_upload(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char filename[64];
    char filesize[32];
    char tmp[32];
    char res[256];

    int ret = sscanf(req, "%s %s %s %s\r\n", cmd, filename, filesize, tmp);
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

    if (!file_service_check_filename(filename)) {
        snprintf(res, sizeof(res), "422 INVALID_FILE_NAME\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    ret = file_service_upload(clientfd, filename, atoi(filesize), session);
    if (ret == ERR_ALREADY_EXISTS) {
        snprintf(res, sizeof(res), "409 FILE_ALREADY_EXISTS\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Gửi thành công
    snprintf(res, sizeof(res), "200 UPLOAD_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}