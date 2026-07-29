#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../../include/client.h"
#include "../../include/protocols.h"
#include "../../include/transport.h"

SSL *g_ssl_table[MAX_CLIENT];

void handle_client(int clientfd, SSL *ssl) {
    session_t session;
    memset(&session, 0, sizeof(session));

    session.logged_in = 0;
    session.user_id = -1;
    session.current_folder_id = -1;
    // init sharing mode
    session.in_sharing_mode = 0;
    session.share_root_folder_id = -1;
    session.old_folder_id = -1;

    session.ssl = ssl;

    char buf[BUFSIZ];
    char cmd[32];

    // Nhận request từ người dùng và phản hồi
    while (1) {
        memset(buf, 0, BUFSIZ);

        int n = net_recv(clientfd, buf, BUFSIZ, 0);

        // client disconnected
        if (n <= 0) break;

        buf[n] = '\0';
        sscanf(buf, "%s", cmd);

        if (strcmp(cmd, "REGISTER") == 0) {
            handle_register(clientfd, buf);
        } else if (strcmp(cmd, "LOGIN") == 0) {
            handle_login(clientfd, buf, &session);
        } else if (strcmp(cmd, "LOGOUT") == 0) {
            handle_logout(clientfd, buf, &session);
        } else if (strcmp(cmd, "PWD") == 0) {
            handle_pwd(clientfd, buf, &session);
        } else if (strcmp(cmd, "LIST") == 0) {
            handle_list(clientfd, buf, &session);
        } else if (strcmp(cmd, "CD") == 0) {
            handle_cd(clientfd, buf, &session); 
        } else if (strcmp(cmd, "MKDIR") == 0) {
            handle_mkdir(clientfd, buf, &session);
        } else if (strcmp(cmd, "DELETE") == 0) {
            handle_delete(clientfd, buf, &session);
        } else if (strcmp(cmd, "RENAME") == 0) {
            handle_rename(clientfd, buf, &session);
        } else if (strcmp(cmd, "RMDIR") == 0) {
            handle_rmdir(clientfd, buf, &session);
        } else if (strcmp(cmd, "UPLOAD") == 0) {
            handle_upload(clientfd, buf, &session);
        } else if (strcmp(cmd, "DOWNLOAD") == 0) {
            handle_download(clientfd, buf, &session);
        } else if (strcmp(cmd, "SHARE_FILE") == 0) {
            handle_share_file(clientfd, buf, &session);
        } else if (strcmp(cmd, "SHARE_FOLDER") == 0) {
            handle_share_folder(clientfd, buf, &session);
        } else if (strcmp(cmd, "UNSHARE_FILE") == 0) {
            handle_unshare_file(clientfd, buf, &session);
        } else if (strcmp(cmd, "UNSHARE_FOLDER") == 0) {
            handle_unshare_folder(clientfd, buf, &session);
        } else if (strcmp(cmd, "LIST_SHARED_USERS_FILE") == 0) {
            handle_list_share_file(clientfd, buf, &session);
        } else if (strcmp(cmd, "LIST_SHARED_USERS_FOLDER") == 0) {
            handle_list_share_folder(clientfd, buf, &session);
        } else if (strcmp(cmd, "LIST_SHARED_FILES_WITH_ME") == 0) {
            handle_list_shared_files_with_me(clientfd, buf, &session);
        } else if (strcmp(cmd, "LIST_SHARED_FOLDERS_WITH_ME") == 0) {
            handle_list_shared_folders_with_me(clientfd, buf, &session);
        } else if (strcmp(cmd, "DOWNLOAD_SHARED") == 0) {
            handle_download_share(clientfd, buf, &session);
        } else if (strcmp(cmd, "OPEN_SHARED_FOLDER") == 0) {
            handle_open_shared_folder(clientfd, buf, &session);
        } else if (strcmp(cmd, "EXIT_SHARED") == 0) {
            handle_exit_shared(clientfd, buf, &session);
        } else {
            // Unsupported protocol
            handle_unsupported(clientfd, buf, &session);
        }
    }
}