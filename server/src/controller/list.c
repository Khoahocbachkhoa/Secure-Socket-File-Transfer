#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"
#include "../../include/db_folder.h"
#include "../../include/db_files.h"

void handle_list(int clientfd, const char *req, session_t *session) {
    char res[256];
    char tmp[256];
    char cmd[16];

    int ret = sscanf(req, "%s %s\r\n", cmd, tmp);
    if (ret != 1) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "401 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Kiểm tra trạng thái của thư mục đó
    ret = db_folder_check_status(session->current_folder_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 SERV_ERROR\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_FOLDER_DELETED) {
        snprintf(res, sizeof(res), "402 FOLDER_DELETED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Bắt đầu gửi nội dung
    snprintf(res, sizeof(res), "200 LIST_BEGIN\r\n");
    net_send(clientfd, res, strlen(res), 0);

    size_t maxlen = MAXENTRYSIZE;
    Entry entries[MAXENTRYSIZE];
    char line[512];

    snprintf(line, sizeof(line), "\n");
    net_send(clientfd, line, strlen(line), 0);
    
    // Gửi các files
    ret = db_file_list(session->current_folder_id, entries, &maxlen);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "\r\n500 SERV_ERROR\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    for (size_t i = 0; i < maxlen; ++i) {
        snprintf(line, sizeof(line), "File\t%s\t%s\t%s\n", 
                entries[i].name, entries[i].owner, entries[i].updated_at);
        
        net_send(clientfd, line, strlen(line), 0);
    }

    // Gửi các folders
    maxlen = MAXENTRYSIZE;
    ret = db_folder_list(session->current_folder_id, entries, &maxlen);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "\r\n500 SERV_ERROR\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    for (size_t i = 0; i < maxlen; ++i) {
        snprintf(line, sizeof(line), "Folder\t%s\t%s\t%s\n", 
                entries[i].name, entries[i].owner, entries[i].updated_at);

        net_send(clientfd, line, strlen(line), 0);
    }

    // Gửi 200 LIST_END báo hiệu kết thúc
    snprintf(res, sizeof(res), "\r\n200 LIST_END\r\n");
    net_send(clientfd, res, strlen(res), 0);
    return;
}