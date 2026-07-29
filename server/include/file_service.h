#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "client.h"
#include <stdbool.h>
#include "database.h"

// Kiểm tra tính hợp lệ của tên file
bool file_service_check_filename(char *fname);

// Service upload một file
db_errror_code file_service_upload(int clientfd, char *fname, int fsize, session_t *session);

// Service download một file
db_errror_code file_service_download(int clientfd, char *fname, session_t *session);

// Download một file bằng fid thay vì fname
db_errror_code file_service_download_by_file_id(int clientfd, int fid, session_t *session);

#endif