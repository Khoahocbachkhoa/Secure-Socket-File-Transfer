#ifndef DB_FOLDER_H
#define DB_FOLDER_H

#include "database.h"
#include <stdbool.h>

#define MAXENTRYSIZE 128

typedef enum {
    ENTRY_FILE,
    ENTRY_FOLDER
} EntryType;

typedef struct {
    long id;

    EntryType type;

    char name[256];

    char owner[64];

    char updated_at[128];

    char role[16];
} Entry;

// Tạo thư mục root khi tạo user
db_errror_code db_folder_create_root(char *username);

// Tạo thư mục mới có cha là id với tên dirname
db_errror_code db_folder_create_new_folder(int folder_id, int owner_id, char *fname);

// Tìm id của thư mục gốc của user có thư mục gốc
db_errror_code db_folder_find_root(int user_id, long *root_id);

// Liệt kê nội dung có trong thư mục đó
db_errror_code db_folder_list(int folder_id, Entry *entries, size_t *max_entries_len);

// Kiểm tra trạng thái của một folder (khả dụng, bị đánh dấu là xóa, không tồn tại trong db nữa)
db_errror_code db_folder_check_status(int folder_id);

// Tìm folder cha
db_errror_code db_folder_find_parent(int folder_id, int *id);

// Tìm folder có tên dirname trong một folder
db_errror_code db_folder_find_folder_by_name(int folder_id, char *dirname, int *id);

// Kiểm tra xem có trong thư mục này đã có 1 folder nào đó tên dirname chưa
db_errror_code db_folder_check_exist_dirname(int folder_id, char *fname, bool *flag);

// Kiểm tra thư mục có rỗng không
db_errror_code db_folder_is_empty(int folder_id);

// Đánh dấu là xóa 1 thư mục
db_errror_code db_folder_soft_delete(int folder_id);

// Đổi tên 1 folder name
db_errror_code db_folder_rename(int folder_id, const char *new_name);

#endif
