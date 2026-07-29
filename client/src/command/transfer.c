#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../include/app.h"
#include "../../include/command.h"
#include "../../include/parser.h"
#include "../../include/transport.h"
#include "../../include/state.h"
#include "../../include/util.h"

void cmd_upload(ParsedCommand *cmd) {
    if (cmd->argc != 3) {
        printf("Usage: put --path path_to_file\n");
        return;
    }

    if (strcmp(cmd->argv[1], "--path") != 0) {
        printf("Error: unknown option '%s'\n", cmd->argv[1]);
        return;
    }

    char *path = cmd->argv[2];
    // chuẩn hóa path
    char fullpath[512];
    normalize_path(path, fullpath, sizeof(fullpath));

    // Lấy tên file
    char filename[256];
    get_file_name(fullpath, filename, sizeof(filename));

    // validate filename
    if (check_file_name(filename) == false) {
        printf("Invalid filename\n");
        return;
    }

    // Mở file
    FILE *fp = fopen(fullpath, "rb");
    
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    // Lấy kích thước file
    fseek(fp, 0, SEEK_END);

    long filesize = ftell(fp);

    rewind(fp);

    if (filesize < 0) {
        printf("Error: cannot determine file size\n");
        fclose(fp);
        return;
    }

    // Gửi request upload
    char req[512];

    snprintf(req, sizeof(req), "UPLOAD %s %ld\r\n",
            filename, filesize);

    send_command(state->sockfd, req);

    // Đọc phản hồi
    char res[256];

    if (recv_response_to_buf(state->sockfd, res, sizeof(res)) < 0) {
        fclose(fp);
        return;
    }

    // parse response, chỉ stream file khi code là 150 Oke
    int code;
    sscanf(res, "%d", &code);

    if (code != 150) {
        printf("%s", res);
        fclose(fp);
        return;
    }

    // Bắt đầu stream file
    char buffer[8192];
    size_t nread;

    while ((nread = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        // Đảm bảo gửi đủ dữ liệu
        if (net_send_all(state->sockfd, buffer, nread, 0) <= 0) {
            printf("Upload failed\nSome error occured or lost connection to server\n");
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    // Wait for upload success
    recv_response(state->sockfd);
}

void cmd_download(ParsedCommand *cmd) {
    if (cmd->argc != 2 && cmd->argc != 4) {
        printf("Usage: get <filename> [--path saved_path]\n");
        return;
    }

    // check argvs
    const char *filename = cmd->argv[1];

    if (!check_file_name(filename)) {
        printf("Error: invalid filename\n");
        return;
    }

    // thư mục lưu trữ
    char *tmp = "~/Downloads";

    if (cmd->argc == 4) {
        if (strcmp(cmd->argv[2], "--path") != 0) {
            printf("Unknown option: %s\n", cmd->argv[2]);
            return;
        }
        tmp = cmd->argv[3];
    }

    char fullpath[256];
    normalize_path(tmp, fullpath, sizeof(fullpath));

    char savepath[512];
    snprintf(savepath, sizeof(savepath), "%s/%s", fullpath, filename);

    // Gửi request
    char req[256];
    snprintf(req, sizeof(req), "DOWNLOAD %s\r\n", filename);

    send_command(state->sockfd, req);

    char res[256];
    // Đọc dòng phản hồi đầu
    recv_response_to_buf(state->sockfd, res, sizeof(res));

    int code;
    char message[32];
    long filesize;

    // ko co filesize -> co error
    if (sscanf(res, "%d %s %ld\r\n", &code, message, &filesize) != 3 || code != 50) {
        printf("%s", res);
        return;
    }

    // Mỏ file đích
    FILE *fp = fopen(savepath, "wb");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    // Tải file về máy
    long received = 0;
    char buffer[8192];

    while (received < filesize) {
        size_t need = sizeof(buffer);

        if ((long)need > (filesize - received))
            need = filesize - received;

        int n = net_recv_all(state->sockfd, buffer, need, 0);
        if (n < 0) {
            printf("Download interrupted\nMaybe some error occured or lost connection to server\n");
            // Xóa file tránh file hỏng
            remove(savepath);
            fclose(fp);
            return;
        }

        received += n;

        // Ghi ra file
        fwrite(buffer, 1, n, fp);
    }

    fclose(fp);
    printf("Saved to path: %s\n", savepath);

    // Đợi phản hồi success từ server
    recv_response(state->sockfd);
}

void cmd_download_shared(ParsedCommand *cmd) {
    if (cmd->argc != 3 && cmd->argc != 5) {
        printf("Usage: get-shared <fileid> <savename> [--path <save_dir>]\n");
        return;
    }

    int fileid = atoi(cmd->argv[1]);

    if (fileid <= 0) {
        printf("Invalid file id\n");
        return;
    }

    const char *savename = cmd->argv[2];
    if (!check_file_name(savename)) {
        printf("Invalid filename\n");
        return;
    }

    char *tmp = "~/Downloads";

    if (cmd->argc == 5) {
        if (strcmp(cmd->argv[3], "--path") != 0) {
            printf("Unknown option: %s\n", cmd->argv[3]);
            return;
        }

        tmp = cmd->argv[4];
    }

    char fullpath[512];

    normalize_path(tmp, fullpath, sizeof(fullpath));

    char savepath[1024];

    snprintf(savepath, sizeof(savepath), "%s/%s", fullpath, savename);

    // download file
    char req[256];

    snprintf(req, sizeof(req), "DOWNLOAD_SHARED %d\r\n", fileid);

    send_command(state->sockfd, req);

    char res[256];

    if (recv_response_to_buf(state->sockfd, res, sizeof(res)) < 0) {
        printf("Connection lost\n");
        return;
    }

    int code;
    char message[64];
    long filesize;

    // Có lỗi xảy ra khi không có file_size
    if (sscanf(res, "%d %s %ld\r\n", &code, message, &filesize) != 3 || code != 50) {
        printf("%s", res);
        return;
    }

    FILE *fp = fopen(savepath, "wb");

    if (fp == NULL) {
        perror("fopen");
        return;
    }

    long received = 0;
    char buffer[8192];

    while (received < filesize) {
        size_t need = sizeof(buffer);

        if ((long)need > filesize - received) {
            need = filesize - received;
        }

        int n = net_recv_all(state->sockfd, buffer, need, 0);

        if (n <= 0) {
            printf("Download interrupted\n");

            fclose(fp);
            remove(savepath);

            return;
        }

        if (fwrite(buffer, 1, n, fp) != (size_t)n) {
            printf("Write file failed\n");

            fclose(fp);
            remove(savepath);

            return;
        }

        received += n;
    }

    fclose(fp);

    printf("Saved to path: %s\n", savepath);
    // Đợi phản hồi thành công từ server
    recv_response(state->sockfd);
}