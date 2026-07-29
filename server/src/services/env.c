#include <stdio.h>
#include <string.h>

#include "../../include/env.h"

char* load_env(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Không truy cập được .env\n");
        return NULL;
    }

    static char conn_str[512];
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        char *key = strtok(line, "=");
        char *value = strtok(NULL, "");

        if (key && value) {
            if (strcmp(key, "DB_CONN") == 0) {
                strncpy(conn_str, value, sizeof(conn_str) - 1);
                conn_str[sizeof(conn_str) - 1] = '\0';
                fclose(file);
                return conn_str;
            }
        }
    }

    fclose(file);
    return NULL;
}
