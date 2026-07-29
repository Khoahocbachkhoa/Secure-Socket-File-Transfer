#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"
#include "../../include/client.h"

void handle_pwd(int clientfd, const char *req, session_t *session) {
    char cmd[16];
    char res[512];
    char temp[256];

    int ret = sscanf(req, "%s %s\r\n", cmd, temp);
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

    snprintf(res, sizeof(res), "200 %s\r\n", session->cwd);
    net_send(clientfd, res, strlen(res), 0);
    return;
}