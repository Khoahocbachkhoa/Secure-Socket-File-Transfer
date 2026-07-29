#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../include/app.h"
#include "../../include/command.h"
#include "../../include/parser.h"
#include "../../include/transport.h"
#include "../../include/state.h"

// commands đã được định nghĩa ở đâu đó
extern Command commands[];

void cmd_exit(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Error: two many arguments!\n");
        return;
    }
    
    app_shutdown();
    exit(EXIT_SUCCESS);
}

void cmd_help(ParsedCommand *cmd) {
    if (cmd->argc != 1) {
        printf("Error: two many arguments!\n");
        return;
    }

    // Print all supported command
    printf("Supported commands:\n\n");

    for (int i = 0; i < 32; ++i) {
        printf("%-20s", commands[i].name);

        if ((i + 1) % 4 == 0) putchar('\n');
    }

    putchar('\n');
}