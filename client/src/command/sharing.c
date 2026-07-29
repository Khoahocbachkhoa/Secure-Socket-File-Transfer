#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../include/app.h"
#include "../../include/command.h"
#include "../../include/parser.h"
#include "../../include/transport.h"
#include "../../include/state.h"

void cmd_share_file(ParsedCommand *cmd) {
    if (cmd->argc != 3) {
        printf("Usage: share-file <filename> <username>\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "SHARE_FILE %s %s\r\n",
            cmd->argv[1], cmd->argv[2]);

    send_command(state->sockfd, req);
    recv_response(state->sockfd);
}

void cmd_unshare_file(ParsedCommand *cmd) {
    if (cmd->argc != 3) {
        printf("Usage: unshare-file <filename> <username>\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "UNSHARE_FILE %s %s\r\n",
            cmd->argv[1], cmd->argv[2]);

    send_command(state->sockfd, req);
    recv_response(state->sockfd);
}

void cmd_share_folder(ParsedCommand *cmd) {
    if (cmd->argc != 4) {
        printf("Usage: share-folder <folder> <username> <role>\n");
        printf("Role must be 'viewer' or 'editor'\n");
        return;
    }

    if (strcmp(cmd->argv[3], "viewer") != 0 &&
        strcmp(cmd->argv[3], "editor") != 0) {

        printf("Error: invalid role\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "SHARE_FOLDER %s %s %s\r\n",
            cmd->argv[1], cmd->argv[2], cmd->argv[3]);

    send_command(state->sockfd, req);
    recv_response(state->sockfd);
}

void cmd_unshare_folder(ParsedCommand *cmd) {
    if (cmd->argc != 3) {
        printf("Usage: unshare-folder <folder> <username>\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req),  "UNSHARE_FOLDER %s %s\r\n",
            cmd->argv[1], cmd->argv[2]);

    send_command(state->sockfd, req);
    recv_response(state->sockfd);
}

void cmd_list_user_access_file(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: file-acl <filename>\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "LIST_SHARED_USERS_FILE %s\r\n",
            cmd->argv[1]);
        
    send_command(state->sockfd, req);

    // Nhận phản hồi từ server
    char buf[512];
    recv_response_to_buf(state->sockfd, buf, sizeof(buf));

    // parse response
    int code;
    sscanf(buf, "%d", &code);

    if (code != 200) {
        // Error list user access file
        printf("%s", buf);
        return;
    }

    // Đọc cho tới dòng cuối
    recv_multiline_reponse(state->sockfd, "200 LIST_SHARED_USERS_END\r\n");
}

void cmd_list_user_access_folder(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: folder-acl <foldername>\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "LIST_SHARED_USERS_FOLDER %s\r\n",
            cmd->argv[1]);
        
    send_command(state->sockfd, req);

    // Nhận phản hồi từ server
    char buf[512];
    recv_response_to_buf(state->sockfd, buf, sizeof(buf));

    // parse response
    int code;
    sscanf(buf, "%d", &code);

    if (code != 200) {
        // Error list user access file
        printf("%s", buf);
        return;
    }

    // Đọc cho tới dòng cuối
    recv_multiline_reponse(state->sockfd, "200 LIST_SHARED_USERS_END\r\n");
}