#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../../include/security.h"

void read_password(char *buffer, size_t size) {
    struct termios oldt;
    struct termios newt;

    //get terminal attributes
    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;

    // disable echo
    newt.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // prompt
    fflush(stdout);

    // read password
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
    }

    // restore terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    printf("\n");

    // remove newline
    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }
}