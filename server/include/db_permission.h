#ifndef DB_PERMISSION_H
#define DB_PERMISSION_H

#include "database.h"

// Kiểm tra xem user_id có quyền được tải file_id không
db_errror_code db_permission_check_access_file(int user_id, int file_id);

// Kiểm tra user_id có quyền truy cập folder_id không
db_errror_code db_permission_check_access_folder(int user_id, int file_id);

#endif