#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

#include "../../include/parser.h"
#include "../../include/util.h"

bool check_folder_name(const char *fname) {
    if (fname == NULL)
        return false;

    size_t len = strlen(fname);

    if (len == 0 || len > 255)
        return false;

    if (strcmp(fname, ".") == 0 ||
        strcmp(fname, "..") == 0)
        return false;

    for (size_t i = 0; i < len; i++) {
        if (fname[i] == '/')
            return false;
    }

    return true;
}

bool check_file_name(const char *fname) {
    if (fname == NULL)
        return false;

    size_t len = strlen(fname);

    if (len == 0 || len > 255)
        return false;

    if (strcmp(fname, ".") == 0 ||
        strcmp(fname, "..") == 0)
        return false;

    for (size_t i = 0; i < len; i++) {
        if (fname[i] == '/')
            return false;
    }

    return true;
}

// Đổi ~ thành home folder
void normalize_path(const char *path, char *buf, int buf_size) {
    if (path == NULL || buf == NULL || buf_size <= 0)
        return;

    if (path[0] == '~') {
        const char *home = getenv("HOME");

        if (home == NULL) {
            struct passwd *pw = getpwuid(getuid());

            if (pw != NULL)
                home = pw->pw_dir;
        }

        if (home == NULL) {
            snprintf(buf, buf_size, "%s", path);
            return;
        }

        snprintf(buf, buf_size, "%s%s", home, path + 1);
    }
    else {
        snprintf(buf, buf_size, "%s", path);
    }
}

void get_file_name(const char *path, char *buf, int buf_size) {
    if (path == NULL || buf == NULL || buf_size <= 0)
        return;

    const char *filename = strrchr(path, '/');

    if (filename == NULL) {
        filename = path;
    } else {
        filename++;
    }

    snprintf(buf, buf_size, "%s", filename);
}