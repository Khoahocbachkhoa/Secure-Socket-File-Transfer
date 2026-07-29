#ifndef DB_SHARING_H
#define DB_SHARING_H

#include "client.h"
#include "database.h"
#include "db_folder.h"

typedef struct {
    char username[64];
    char role[16];
} shared_user_t;

typedef struct {
    int owner_id;
    char owner_name[256];
    char role[16];
    char folder_name[256];
} access_info_t;

// cấp quyền truy cập file cho 1 user
db_errror_code db_sharing_grant_file_access(int file_id, int user_id);

// hủy quyền chia sẻ file cho 1 user
db_errror_code db_sharing_revoke_file_access(int file_id, int user_id);

// Xem cac user nao dang co quyen truy cap file
db_errror_code db_sharing_list_users_shared(int file_id, char **users, int *users_size);

// Xem các user nào đang chia sẻ file cho tôi
db_errror_code db_sharing_list_files_shared_with_me(int user_id, Entry *entries, int *len);

// Cấp quyền truy cập folder cho 1 user
db_errror_code db_sharing_grant_folder_access(int folder_id, int user_id, char *role);

// hủy quyền chia sẻ folder cho 1 user
db_errror_code db_sharing_revoke_folder_access(int folder_id, int user_id);

// Xem các user nào đang có quyền truy cập folder
db_errror_code db_sharing_list_users_shared_folder(int folder_id, shared_user_t *user, int *user_size);

// Xem các user nào đang chia sẻ folder cho tôi
db_errror_code db_sharing_list_folders_shared_with_me(int user_id, Entry *entries, int *len);

// Lấy thông tin của thư mục được chia sẻ với tôi
db_errror_code db_sharing_get_folder_access_info(int folder_id, access_info_t *info, session_t *session);

#endif