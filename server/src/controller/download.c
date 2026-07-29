#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/database.h"
#include "../../include/file_service.h"

void handle_download(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char filename[64];
    char tmp[32];
    char res[256];

    int ret = sscanf(req, "%s %s %s\r\n", cmd, filename, tmp);
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

    // Gọi service để tải file
    ret = file_service_download(clientfd, filename, session);
    if (ret == DB_FILE_NOT_FOUND) {
        snprintf(res, sizeof(res), "404 FILE_NOT_FOUND\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == ERR) {
        snprintf(res, strlen(res), "500 ERR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Gửi thành công
    snprintf(res, sizeof(res), "200 DOWNLOAD_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}
