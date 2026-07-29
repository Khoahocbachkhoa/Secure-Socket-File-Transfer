#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"
#include "../../include/db_auth.h"
#include "../../include/db_folder.h"
#include "../../include/security.h"

void handle_login(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char username[64];
    char password[64];
    char res[256];
    int *id = (int*)malloc(sizeof(int));

    if (id == NULL) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    int ret = sscanf(req, "%s %s %s\r\n", cmd, username, password);
    if (ret != 3) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (session->logged_in) {
        snprintf(res, sizeof(res), "402 ALREADY_LOGIN\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Băm mật khẩu
    char hash[256];
    password_hash(password, hash);

    ret = db_check_login(username, hash, id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == DB_AUTH_INVALID_CREDENTIALS) {
        snprintf(res, sizeof(res), "401 INVALID_CREDENTIALS\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Login oke
    session->logged_in = 1;
    strncpy(session->username, username, sizeof(session->username) - 1);
    session->username[sizeof(session->username) - 1] = '\0';
    session->user_id = *id;
    strcpy(session->cwd , "/");

    // Thiết lập thư mục ban đầu khi login là /
    ret = db_folder_find_root(session->user_id, &session->current_folder_id);
    
    if (ret == ERR || ret == DB_USER_NOT_FOUND) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (ret == OK) {
        snprintf(res, sizeof(res), "200 LOGIN_SUCCESS\r\n");
        net_send(clientfd, res, strlen(res), 0);
    }

    free(id);
}