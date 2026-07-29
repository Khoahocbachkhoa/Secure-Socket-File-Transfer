#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "../../include/file_service.h"
#include "../../include/database.h"
#include "../../include/db_files.h"
#include "../../include/transport.h"
#include "../../include/storage.h"
#include "../../include/db_permission.h"

bool file_service_check_filename(char *name) {
    if (name == NULL || *name == '\0')
        return false;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return false;

    for (char *p = name; *p; ++p) {
        // no separator
        if (*p == '/' || *p == '\\')
            return false;

        // no control
        if (iscntrl((unsigned char)*p))
            return false;
    }

    return true;
}

db_errror_code file_service_upload(int clientfd, char *fname, int fsize, session_t *session) {
    if (fname == NULL)
        return ERR_INVALID_ARGUMENT;

    // Kiểm tra xem file đã tồn tại chưa
    int ret = db_file_exists(session->current_folder_id, fname);
    if (ret == DB_FILE_EXISTS) {
        return ERR_ALREADY_EXISTS;
    }

    // Tạo storage_key để lưu trữ
    char storage_key[128];

    snprintf(storage_key, sizeof(storage_key), "%d_%ld_%u_%s", session->user_id, time(NULL), (unsigned)rand(), fname);

    // Tạo storage path
    char fullpath[256];
    
    //snprintf(fullpath, sizeof(fullpath), "./storage/%s/%s", session->username, storage_key);
    snprintf(fullpath, sizeof(fullpath), "./storage/%s", storage_key);

    // Báo cho client bắt đầu gửi data
    char res[128];
    snprintf(res, sizeof(res), "150 READY_TO_RECEIVE\r\n");
    net_send(clientfd, res, strlen(res), 0);

    // Gọi service để lưu file xuống disk
    int ok = storage_save_file(fullpath, clientfd, fsize);
    if (!ok) {
        return ERR;
    }

    // Lưu metadata
    ret = db_file_insert(fname, storage_key, fsize, session);
    if (ret == ERR) {
        // Xóa đi
        remove(fullpath);
        return ERR;
    }

    return OK;
}

db_errror_code file_service_download(int clientfd, char *fname, session_t *session) {
    if (fname == NULL)
        return ERR_INVALID_ARGUMENT;

    // Kiểm tra xem file đã tồn tại chưa
    // int ret = db_file_exists(session->current_folder_id, fname);
    // if (ret == DB_FILE_NOT_FOUND) {
    //     return ERR_NOT_FOUND;
    // }

    // Lấy metadata của file
    FileMeta meta;
    int ret = db_file_find_by_name(session->current_folder_id, fname, &meta);
    if (ret != OK) {
        return ret;
    }

    // Bắt đầu gửi file
    char res[256];
    snprintf(res, sizeof(res), "50 START_DOWNLOAD %ld\r\n", meta.size);
    net_send(clientfd, res, strlen(res), 0);

    // Gọi service để gửi file từ disk cho client
    char path[512];
    
    //snprintf(path, sizeof(path), "./storage/%s/%s", session->username, meta.storage_key);
    snprintf(path, sizeof(path), "./storage/%s", meta.storage_key);

    bool ok = storage_send_file(path, clientfd, meta.size);

    if (!ok) {
        return ERR;
    }

    return OK;
}

db_errror_code file_service_download_by_file_id(int clientfd, int fid, session_t *session) {
    // Kiểm tra truy nhập
    int ret;

    ret = db_permission_check_access_file(session->user_id, fid);
    if (ret != OK) {
        return ERR;
    }
    
    // Lấy metadata của file
    FileMeta meta;
    ret = db_file_find_by_id(fid, &meta);
    if (ret != OK) {
        return ret;
    }

    // Bắt đầu gửi file
    char res[256];
    snprintf(res, sizeof(res), "50 START_DOWNLOAD %ld\r\n", meta.size);
    net_send(clientfd, res, strlen(res), 0);

    // Gọi service để gửi file từ disk cho client
    char path[512];
    
    //snprintf(path, sizeof(path), "./storage/%s/%s", session->username, meta.storage_key);
    snprintf(path, sizeof(path), "./storage/%s", meta.storage_key);

    bool ok = storage_send_file(path, clientfd, meta.size);

    if (!ok) {
        return ERR;
    }

    return OK;
}