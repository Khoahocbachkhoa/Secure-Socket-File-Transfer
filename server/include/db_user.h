#ifndef DB_USER_H
#define DB_USER_H

#include "database.h"

// Kiểm tra user có tồn tại hay chưa
db_errror_code db_user_exists(const char *username);

// Tạo user mới
db_errror_code db_user_insert(const char *username, const char *password);

// Trả về id của 1 username
db_errror_code db_user_find_id_by_username(const char *username, int *id);

#endif