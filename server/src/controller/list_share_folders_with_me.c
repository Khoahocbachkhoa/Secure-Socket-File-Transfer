#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/protocols.h"
#include "../../include/db_folder.h"
#include "../../include/db_sharing.h"
#include "../../include/transport.h"

void handle_list_shared_folders_with_me(int clientfd, const char *req,  session_t *session) {
    char res[256];
    int ret;

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "401 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    Entry entries[256];
    int n = 256;

    ret = db_sharing_list_folders_shared_with_me(
            session->user_id,
            entries,
            &n);

    if (ret != OK) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    snprintf(res, sizeof(res), "200 LIST_SHARED_WITH_ME_BEGIN\r\n");
    net_send(clientfd, res, strlen(res), 0);

    char buf[512];

    for (int i = 0; i < n; i++) {
        snprintf(buf, sizeof(buf), "FOLDER %ld %s %s %s\r\n",
            entries[i].id, entries[i].name, entries[i].owner, entries[i].role);

        net_send(clientfd, buf, strlen(buf), 0);
    }

    snprintf(res, sizeof(res), "200 LIST_SHARED_WITH_ME_END\r\n");
    net_send(clientfd, res, strlen(res), 0);
}