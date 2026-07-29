#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "../../include/protocols.h"
#include "../../include/security.h"
#include "../../include/transport.h"
#include "../../include/db_user.h"
#include "../../include/db_folder.h"
#include "../../include/database.h"

void handle_register(int clientfd, const char *req) {
    char cmd[16];
    char username[64];
    char password[64];
    char res[256];

    int ret = sscanf(req, "%s %s %s\r\n", cmd, username, password);
    if (ret != 3) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Check password đủ mạnh không
    ret = check_password(password);
    if (ret == PASSWORD_WEAK) {
        snprintf(res, sizeof(res), "422 WEAK_PASSWORD\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    } else if (ret == PASSWORD_INVALID_ARGUMENT) {
        snprintf(res, sizeof(res), "400 BAD_REQUEST\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Check user có tồn tại chưa
    ret = db_user_exists(username);
    if (ret == DB_USER_EXISTS) {
        snprintf(res, sizeof(res), "409 USER_EXISTS\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Băm mật khẩu bằng SHA256
    char hash[65];

    password_hash(password, hash);

    // Tạo user mới
    ret = db_user_insert(username, hash);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // tạo thư mục root cho user vừa tạo
    ret = db_folder_create_root(username);
    if (ret == ERR) {
        snprintf(res, sizeof(res), "500 ERROR_SERVER\r\n");
        net_send(clientfd, res, strlen(res), 0);
        return;
    }

    // Tạo thư mục mới trên đĩa cho user
    char path[256];
    snprintf(path, sizeof(path), "./storage/%s", username);
    // mkdir(path, 0755);

    // Tạo user mới và thêm folder lưu trữ thành công
    snprintf(res, sizeof(res), "200 REGISTER_SUCCESS\r\n");
    net_send(clientfd, res, strlen(res), 0);

    return;
}