#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "../../include/app.h"
#include "../../include/command.h"
#include "../../include/parser.h"
#include "../../include/transport.h"
#include "../../include/state.h"
#include "../../include/util.h"

void cmd_pwd(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Usage: pwd\n");
        return;
    }

    send_command(state->sockfd, "PWD\r\n");

    // In phản hồi từ server
    recv_response(state->sockfd);
}

void cmd_list(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Usage: list\n");
        return;
    }

    send_command(state->sockfd, "LIST\r\n");

    // In phản hồi từ server
    // recv_response(state->sockfd);

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

    // Doc nhieu dong cho toi khi gap 200 
    recv_multiline_reponse(state->sockfd, "200 LIST_END\r\n");
}

void cmd_mkdir(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: mkdir <folder_name>\n(Only support one dir per command!)\n");
        return;
    }

    char req[256];
    char *folder_name = cmd->argv[1];

    if (strlen(folder_name) > 255) {
        printf("Error: folder name length too long!\n");
        return;
    }

    // Kiểm tra tên có hợp lệ không
    if (check_folder_name(folder_name) == false) {
        printf("Error: folder name is not valid!\n");
        return;
    }

    snprintf(req, sizeof(req), "MKDIR %s\r\n", folder_name);
    send_command(state->sockfd, req);

    // Nhận phản hồi từ server
    recv_response(state->sockfd);
}

void cmd_cd(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: cd <path>\n(Only support single path traversal and ..)\n");
        return;
    }

    char req[256];
    char *path = cmd->argv[1];

    // Kiểm tra path có hợp lệ không
    if ((strcmp(path, "..") != 0) && (check_folder_name(path) == false)) {
        printf("Error: path name not valid!\n");
        return;
    }

    snprintf(req, sizeof(req), "CD %s\r\n", path);

    // Gửi lệnh tới server
    send_command(state->sockfd, req);

    // Nhận phản hồi
    recv_response(state->sockfd);

    // Gửi pwd tới server
    send_command(state->sockfd, "PWD\r\n");

    // Cập nhật cwd
    // 200 dir_name
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

void cmd_delete(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: rm <file>\n");
        return;
    }

    const char *filename = cmd->argv[1];

    if (check_file_name(filename) == false) {
        printf("Error: invalid file name!\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "DELETE %s\r\n", filename);

    send_command(state->sockfd, req);

    recv_response(state->sockfd);
}

void cmd_rmdir(ParsedCommand *cmd) {
    if (cmd->argc != 2) {
        printf("Usage: rmdir <folder>\n");
        return;
    }

    const char *dirname = cmd->argv[1];

    if (check_folder_name(dirname) == false) {
        printf("Error: invalid folder name!\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "RMDIR %s\r\n", dirname);

    send_command(state->sockfd, req);

    recv_response(state->sockfd);
}

void cmd_rename(ParsedCommand *cmd) {
    if (cmd->argc != 3) {
        printf("Usage: mv <old_name> <new_name>\n");
        return;
    }

    const char *old_name = cmd->argv[1];
    const char *new_name = cmd->argv[2];

    if (strlen(old_name) == 0 || strlen(new_name) == 0) {
        printf("Error: invalid name!\n");
        return;
    }

    char req[512];

    snprintf(req, sizeof(req), "RENAME %s %s\r\n", old_name, new_name);

    send_command(state->sockfd, req);

    recv_response(state->sockfd);
}