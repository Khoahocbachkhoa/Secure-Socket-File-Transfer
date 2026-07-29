#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 16

typedef struct
{
    int argc;
    char *argv[MAX_ARGS];

} ParsedCommand;

// Phần tích một dòng thành chuỗi các token
int parseLine(char *line, ParsedCommand *cmd);

#endif