#ifndef DB_FILE_H
#define DB_FILE_H

#include "database.h"
#include "db_folder.h"
#include "client.h"

typedef struct {
    long id;
    long owner_id;
    char filename[256];
    char storage_key[256];
    long size;
} FileMeta;

// Liệt kê các file có trong thư mục có folder_id
db_errror_code db_file_list(int folder_id, Entry *entries, size_t *max_entries_len);

// Kiểm tra một tên file đã tồn tại trong thư mục hay chưa
db_errror_code db_file_exists(int fid, const char *fname);

// Thêm một meta data
db_errror_code db_file_insert(char *fname, char *storagekey, int fsize, session_t *session);

// Lấy metadata của một file theo tên file
db_errror_code db_file_find_by_name(int folder_id, char *fname, FileMeta *meta);

// Lấy metadata của một file theo file id
db_errror_code db_file_find_by_id(int fid, FileMeta *meta);

//Lấy file id có filename
db_errror_code db_file_find_id_by_name(int folder_id, char *fname, int *id);

// Lấy storage key của 1 file có file id
db_errror_code db_file_get_storage_key_by_id(int file_id, char *storage_key, int storage_key_size);

// Đánh dấu là đã xóa 1 file
db_errror_code db_file_soft_delete(int file_id);

// Đổi tên 1 file
db_errror_code db_file_rename(int file_id, const char *new_name);

#endif