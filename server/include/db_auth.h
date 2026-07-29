#ifndef DB_AUTH_H
#define DB_AUTH_H

#include "database.h"

// Kiểm tra tên đăng nhập + mật khẩu
db_errror_code db_check_login(const char *username, const char *password, int *id);

#endif