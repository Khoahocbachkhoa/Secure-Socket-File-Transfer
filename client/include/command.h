#ifndef COMMAND_H
#define COMMAND_H

#include "parser.h"

typedef void (*CommandHandler)(
    ParsedCommand *cmd
);

typedef struct
{
    const char *name;
    CommandHandler handler;

} Command;

extern Command commands[];

// dispatch parsed command cho một handle
void command_dispatch(ParsedCommand *cmd);

// Các handler
void cmd_register(ParsedCommand *cmd);
void cmd_login(ParsedCommand *cmd);
void cmd_logout(ParsedCommand *cmd);
void cmd_exit(ParsedCommand *cmd);
void cmd_pwd(ParsedCommand *cmd);
void cmd_list(ParsedCommand *cmd);
void cmd_cd(ParsedCommand *cmd);
void cmd_mkdir(ParsedCommand *cmd);
void cmd_delete(ParsedCommand *cmd);
void cmd_rmdir(ParsedCommand *cmd);
void cmd_rename(ParsedCommand *cmd);
void cmd_upload(ParsedCommand *cmd);
void cmd_download(ParsedCommand *cmd);
void cmd_share_file(ParsedCommand *cmd);
void cmd_unshare_file(ParsedCommand *cmd);
void cmd_share_folder(ParsedCommand *cmd);
void cmd_unshare_folder(ParsedCommand *cmd);
void cmd_list_user_access_file(ParsedCommand *cmd);
void cmd_list_user_access_folder(ParsedCommand *cmd);
void cmd_list_share_file_with_me(ParsedCommand *cmd);
void cmd_list_share_folder_with_me(ParsedCommand *cmd);
void cmd_download_shared(ParsedCommand *cmd);
void cmd_open_shared_folder(ParsedCommand *cmd);
void cmd_exit_shared_folder(ParsedCommand *cmd);
void cmd_help(ParsedCommand *cmd);

#endif