#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../include/app.h"
#include "../../include/command.h"
#include "../../include/parser.h"
#include "../../include/transport.h"
#include "../../include/state.h"

void cmd_list_share_file_with_me(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Usage: shared-files\nList all file you can access through sharing\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "LIST_SHARED_FILES_WITH_ME\r\n");

    send_command(state->sockfd, req);

    // Nhận phản hồi từ server
    char buf[512];
    recv_response_to_buf(state->sockfd, buf, sizeof(buf));

    // parse response
    int code;
    sscanf(buf, "%d", &code);

    if (code != 200) {
        // Error list
        printf("%s", buf);
        return;
    }

    // doc cho toi khi dong cuoi
    recv_multiline_reponse(state->sockfd, "200 LIST_SHARED_WITH_ME_END\r\n");
}

void cmd_list_share_folder_with_me(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Usage: shared-folders\nList all folders you can access through sharing and your roles.\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "LIST_SHARED_FOLDERS_WITH_ME\r\n");

    send_command(state->sockfd, req);

    // Nhận phản hồi từ server
    char buf[512];
    recv_response_to_buf(state->sockfd, buf, sizeof(buf));

    // parse response
    int code;
    sscanf(buf, "%d", &code);

    if (code != 200) {
        // Error list
        printf("%s", buf);
        return;
    }

    // doc cho toi khi dong cuoi
    recv_multiline_reponse(state->sockfd, "200 LIST_SHARED_WITH_ME_END\r\n");
}

void cmd_open_shared_folder(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: open-shared <folder_id>\nYou can get folder_id with cmd: shared_folders\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "OPEN_SHARED_FOLDER %s\r\n",
            cmd->argv[1]);
    
    send_command(state->sockfd, req);
    recv_response(state->sockfd);

    // Gửi pwd tới server
    send_command(state->sockfd, "PWD\r\n");

    // Cập nhật path mới
    char buf[512];
    recv_response_to_buf(state->sockfd, buf, sizeof(buf));

    int code;
    char dir[256];

    // parser status code and dir
    sscanf(buf, "%d %s\r\n", &code, dir);
    if (code != 200) {
        printf("Error: cannot find cwd\n");
        return; 
    }

    // update curent dir
    strcpy(state->cwd, dir);
}

void cmd_exit_shared_folder(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Usage: exit-shared\nExit sharing mode\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "EXIT_SHARED\r\n");

    send_command(state->sockfd, req);
    recv_response(state->sockfd);
}