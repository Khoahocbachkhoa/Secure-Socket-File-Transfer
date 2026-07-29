#include <stdio.h>
#include <stdlib.h>

#include "../../include/database.h"
#include "../../include/db_pool.h"
#include "../../include/db_auth.h"

// Kiểm tra thông tin đăng nhập
db_errror_code db_check_login(const char *username, const char *password, int *id) {
    if (username == NULL || password == NULL) {
        return DB_AUTH_INVALID_CREDENTIALS;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "db_user_exists: pool exhausted\n");
        return ERR;
    }

    const char *query = "select id from users where username = $1 and password = $2;";
    const char *params[2];
    params[0] = username;
    params[1] = password;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    // Không tìm thấy username có password
    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);
        return DB_AUTH_INVALID_CREDENTIALS;
    }

    // Trả về id tìm thấy
    if (id != NULL) {
        *id = atoi(PQgetvalue(res, 0, 0));
    }

    PQclear(res);
    db_release(conn);

    return OK;
}
