#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/command.h"
#include "../../include/parser.h"

Command commands[32] =
{
    {"login",           cmd_login},
    {"logout",          cmd_logout},

    {"register",        cmd_register},

    {"pwd",             cmd_pwd},
    {"ls",              cmd_list},
    {"cd",              cmd_cd},

    {"mkdir",           cmd_mkdir},
    {"rm",              cmd_delete},
    {"rmdir",           cmd_rmdir},
    {"mv",              cmd_rename},

    {"put",             cmd_upload},
    {"get",             cmd_download},

    {"share-file",      cmd_share_file},
    {"unshare-file",    cmd_unshare_file},

    {"share-folder",    cmd_share_folder},
    {"unshare-folder",  cmd_unshare_folder},

    {"file-acl",        cmd_list_user_access_file},
    {"folder-acl",      cmd_list_user_access_folder},

    {"shared-files",    cmd_list_share_file_with_me},
    {"shared-folders",  cmd_list_share_folder_with_me},

    {"get-shared",      cmd_download_shared},

    {"open-shared",     cmd_open_shared_folder},
    {"exit-shared",     cmd_exit_shared_folder},

    {"help",            cmd_help},

    {"exit",            cmd_exit},
    {"quit",            cmd_exit},

    // alias
    {"dir",   cmd_list},
    {"bye",   cmd_exit},
    {"upload", cmd_upload},
    {"download", cmd_download},
    {"cwd", cmd_pwd},
    {"del", cmd_delete}
};

void command_dispatch(ParsedCommand *cmd) {
    size_t cnt = sizeof(commands) / sizeof(commands[0]);

    // look up for handler
    for (size_t i = 0; i < cnt; ++i) {
        if (strcmp(cmd->argv[0], commands[i].name) == 0) {
            // Gọi handler
            commands[i].handler(cmd);
            return;
        }
    }

    // Không tìm thấy command!
    printf("Error: unknown command '%s'\n", cmd->argv[0]);
}