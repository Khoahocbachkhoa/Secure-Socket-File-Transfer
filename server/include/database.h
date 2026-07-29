#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>

typedef enum {
    DB_OK = 0,
    DB_ERR_CONNECTION_FAILED,
    DB_ERR_NOT_CONNECTED,
    DB_ERR_QUERY_FAILED,
    DB_ERR_TABLE_CREATE_FAILED,
    DB_ERR_INVALID_ARGUMENT
} db_status;

typedef enum {
    DB_DISCONNECTED = 0,
    DB_CONNECTED = 1
} db_connection_state;

typedef enum {
    // No error
    OK = 0,
    ERR,

    // generic error
    ERR_INVALID_ARGUMENT,
    ERR_NOT_FOUND,
    ERR_ALREADY_EXISTS,
    ERR_FORBIDDEN,

    // user
    DB_USER_EXISTS,
    DB_USER_NOT_FOUND,
    DB_USER_INVALID_ARGUMENT,
    DB_USER_INVALID_CREDENTIALS,

    // auth
    DB_AUTH_INVALID_ARGUMENT,
    DB_AUTH_INVALID_CREDENTIALS,

    // folder
    DB_FOLDER_DELETED,
    DB_FOLDER_MOVED,
    DB_FOLDER_NOT_FOUND,
    DB_FOLDER_NAME_NOT_FOUND,
    DB_FOLDER_NOT_EMPTY,
    DB_FOLDER_EXISTS,

    // file
    DB_FILE_INVALID_NAME,
    DB_FILE_EXISTS,
    DB_FILE_NOT_FOUND,

    // sharing
    DB_SHARING_NOT_FOUND,

    // permission
    DB_PERMISSION_DENIED
    
} db_errror_code;

extern const char *g_db_conninfo;

// Khởi tạo 1 connection mới
PGconn *db_connect(const char *conninfo);

// Đóng 1 connection
void db_disconnect(PGconn *conn);

// Kiểm tra trạng thái
db_connection_state db_is_connected(PGconn *conn);

#endif