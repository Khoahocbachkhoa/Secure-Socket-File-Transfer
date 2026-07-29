#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"

void handle_logout(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char res[256];

    int ret = sscanf(req, "%s\r\n", cmd);
    if (ret != 1) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "201 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // set session to zero
    memset(session, 0, sizeof(session_t));
    session->logged_in = 0;
    session->user_id = 0;
    session->current_folder_id = -1;

    snprintf(res, sizeof(res), "200 LOGOUT_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);
}
