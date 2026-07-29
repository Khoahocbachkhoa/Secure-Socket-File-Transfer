#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../include/db_pool.h"
#include "../../include/db_folder.h"
#include "../../include/db_user.h"
#include "../../include/database.h"
#include "../../include/db_permission.h"\

db_errror_code db_permission_check_access_file(int user_id, int file_id) {
    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT 1 "
        "FROM file_permissions "
        "WHERE file_id = $1 "
        "AND user_id = $2";

    char file_id_str[16];
    char user_id_str[16];

    snprintf(file_id_str, sizeof(file_id_str), "%d", file_id);

    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[2] = {
        file_id_str,
        user_id_str
    };

    PGresult *res = PQexecParams(
        conn,
        query,
        2,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Permission check failed: %s\n", PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);

    PQclear(res);
    db_release(conn);

    if (rows == 0) {
        return DB_PERMISSION_DENIED;
    }

    return OK;
}

db_errror_code db_permission_check_access_folder(int user_id, int file_id) {
        PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT 1 "
        "FROM folder_permissions "
        "WHERE folder_id = $1 "
        "AND user_id = $2";

    char folder_id_str[16];
    char user_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", file_id);

    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[2] = {
        folder_id_str,
        user_id_str
    };

    PGresult *res = PQexecParams(
        conn,
        query,
        2,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Permission check failed: %s\n", PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);

    PQclear(res);
    db_release(conn);

    if (rows == 0) {
        return DB_PERMISSION_DENIED;
    }

    return OK;
}