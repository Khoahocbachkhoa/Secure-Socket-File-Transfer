#include <stdio.h>
#include <stdlib.h>

#include "../../include/database.h"
#include "../../include/db_pool.h"
#include "../../include/db_user.h"

// Kiểm tra sự tồn tại của user cũ
db_errror_code db_user_exists(const char *username) {
    if (username == NULL)
        return DB_USER_INVALID_ARGUMENT;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "db_user_exists: pool exhausted\n");
        return ERR;
    }

    const char *query = "select count(*) from users where username = $1;";
    const char *params[1];
    params[0] = username;

    // Truy vấn
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "query errror!");
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    // Lấy kết quả trả về
    char *count_str = PQgetvalue(res, 0, 0);
    int cnt = atoi(count_str);

    PQclear(res);
    db_release(conn);

    return (cnt == 0) ? DB_USER_NOT_FOUND : DB_USER_EXISTS;
}

// Thêm một user mới
db_errror_code db_user_insert(const char *username, const char *password) {
    if (username == NULL || password == NULL) {
        return DB_USER_INVALID_ARGUMENT;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "db_user_exists: pool exhausted\n");
        return ERR;
    }

    const char *query = "insert into users(username, password) "
                        "values ($1, $2);";
                    
    const char *params[2];
    params[0] = username;
    params[1] = password;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Lỗi truy vấn(db_insert_user)!");
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);
    return OK;
}

// Trả về ID của một username
db_errror_code db_user_find_id_by_username(const char *username, int *id) {
    if (username == NULL || id == NULL)
        return DB_USER_INVALID_ARGUMENT;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "db_user_find_id_buy_username: pool exhausted\n");
        return ERR;
    }

    const char *query = "select id from users where username = $1;";
    const char *params[1];
    params[0] = username;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "query errror!");
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    // Nếu không tìm thấy user
    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);

        return DB_USER_NOT_FOUND;
    }

    // Lấy id
    *id = atoi(PQgetvalue(res, 0, 0));

    PQclear(res);
    db_release(conn);

    return OK;
}