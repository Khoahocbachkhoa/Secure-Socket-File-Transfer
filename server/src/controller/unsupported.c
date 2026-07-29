#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/transport.h"

void handle_unsupported(int clientfd, const char *req, session_t *session) {
    (void)req;
    (void)session;
    char res[256];
    snprintf(res, sizeof(res), "500 PROTOCOL_NOT_SUPPORTED\r\n");
    net_send(clientfd, res, strlen(res), 0);

    return;
}