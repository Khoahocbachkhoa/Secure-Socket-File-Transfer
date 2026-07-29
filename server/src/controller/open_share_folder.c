#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/client.h"
#include "../../include/protocols.h"
#include "../../include/db_files.h"
#include "../../include/db_sharing.h"
#include "../../include/transport.h"
#include "../../include/db_folder.h"
#include "../../include/db_permission.h"

// typedef struct {
//     int logged_in;
//     int user_id;
//     char username[64];
    
//     long current_folder_id; // Id thu muc hien tai
//     char cwd[256]; // Thu muc lam viec hien tai

//     int in_sharing_mode; // = 1 neu dang o trong folder duoc chia se voi toi

//     int shared_owner_id;
//     char shared_owner[64];

//     share_role_t role;

//     long share_root_folder_id; // Thư mục gốc của phân quyền
//     long old_folder_id; // Thu muc truoc khi kia khi dang khong o trong sharing mode
//     char share_cwd[256]; // Thu muc hien tai khi dang o trong che do chia se
// } session_t;

void handle_open_shared_folder(int clientfd, const char *req, session_t *session) {
    char res[256];
    char cmd[64];
    int folder_id;
    int ret;

    if (session->logged_in == 0) {
        snprintf(res, sizeof(res), "401 NOT_LOGIN_YET\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    ret = sscanf(req, "%s %d\r\n", cmd, &folder_id);
    if (ret != 2) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Kiểm tra xem có quyền không
    ret = db_permission_check_access_folder(session->user_id, folder_id);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (ret == DB_PERMISSION_DENIED) {
        snprintf(res, sizeof(res), "402 PERMISSION_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Thay đổi session_t khi có quyền
    // Lay access_info
    access_info_t info;
    ret = db_sharing_get_folder_access_info(folder_id, &info, session);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    if (ret == DB_PERMISSION_DENIED) {
        snprintf(res, sizeof(res), "402 PERMISSION_DENIED\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }
    

    // Thay doi session để phù hợp hơn
    session->in_sharing_mode = 1;

    session->old_folder_id = session->current_folder_id;

    session->share_root_folder_id = folder_id;

    session->current_folder_id = folder_id;

    session->shared_owner_id = info.owner_id;

    strcpy(session->shared_owner, info.owner_name);

    strcpy(session->old_cwd, session->cwd);

    // Change cwd
    snprintf(session->cwd, sizeof(session->cwd), "%s:/%s", info.owner_name, info.folder_name);

    if (strcmp(info.role, "viewer") == 0)
        session->role = ROLE_VIEWER;
    else
        session->role = ROLE_EDITOR;

    snprintf(res, sizeof(res), "200 OKE\r\n");
    net_send(clientfd, res, strlen(res), 0);
}