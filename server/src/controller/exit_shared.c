#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/client.h"
#include "../../include/protocols.h"
#include "../../include/transport.h"

void handle_exit_shared(int clientfd, const char *req, session_t *session) {
    char res[256];

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "401 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (session->in_sharing_mode == 0) {
        snprintf(res, sizeof(res), "409 NOT_IN_SHARED_MODE\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    /* Khôi phục thư mục trước khi vào workspace share */
    session->current_folder_id =
        session->old_folder_id;

    // restore cwd name
    strcpy(session->cwd, session->old_cwd);

    /* Reset trạng thái sharing */
    session->in_sharing_mode = 0;

    session->shared_owner_id = 0;
    session->shared_owner[0] = '\0';

    session->share_root_folder_id = 0;

    session->old_folder_id = 0;

    snprintf(res, sizeof(res), "200 EXIT_SHARED_SUCCESS\r\n");

    net_send(clientfd, res, strlen(res), 0);
}