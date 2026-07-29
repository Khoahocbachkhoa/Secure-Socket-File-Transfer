#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../../include/db_pool.h"
#include "../../include/db_folder.h"
#include "../../include/db_user.h"
#include "../../include/database.h"

db_errror_code db_folder_create_root(char *username) {
    int id;
    char id_str[32];
    // Tìm kiếm id của user có username
    int ret = db_user_find_id_by_username(username, &id);

    if (ret == ERR) {
        fprintf(stderr, "create root folder error!\n");
        return ERR;
    } else if (ret == DB_USER_NOT_FOUND) {
        fprintf(stderr, "create root folder: username not found!\n");
        return ERR;
    }

    // tạo thư mục gốc cho user có id
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "create root folder: pool exhausted\n");
        return ERR;
    }

    const char *query = 
        "INSERT INTO folders "
        "(owner_id, parent_id, name) "
        "VALUES ($1, NULL, '/');";

    const char *params[1];
    snprintf(id_str, sizeof(id_str), "%d", id);
    params[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "create root folder query error: %s\n", PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_folder_find_root(int user_id, long *root_id) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "find root folder: pool exhausted\n");
        return ERR;
    }

    const char *query = "select id from folders where owner_id = $1 and parent_id is NULL;";
    const char *params[1];
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", user_id);
    params[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Fint root folder error: %s\n", PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Find roor folder: user_id not found!\n");
        PQclear(res);
        db_release(conn);

        return DB_USER_NOT_FOUND;
    }

    // Lay root folder id
    *root_id = atoi(PQgetvalue(res, 0, 0));

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_folder_check_status(int folder_id) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        return ERR;
    }

    const char *query = "select deleted_at from folders where id = $1";
    const char *params[1];
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);
    params[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Check status folder error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Check status folder: folder not found\n");
        PQclear(res);
        db_release(conn);
        return DB_FOLDER_NOT_FOUND;
    }

    int ret;
    if (!PQgetisnull(res, 0, 0)) {
        ret = DB_FOLDER_DELETED;
    } else {
        ret = OK;
    }

    PQclear(res);
    db_release(conn);

    return ret;
}

// Liệt kê nội dung của thư mục có folder_id
db_errror_code db_folder_list(int folder_id, Entry *entries, size_t *maxlen) {
    if (entries == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // int ret = db_folder_check_status(folder_id);
    // if (ret != OK) {
    //     return ret;
    // }

    // Liệt kê các folder trước
    PGconn *conn = db_acquire();
    
    if (conn == NULL) {
        fprintf(stderr, "List : Db exhausted error\n");
        return ERR;
    }

    const char *query = "select f.id, f.name, u.username as ownername, f.updated_at "
        "from folders f join users u on f.owner_id = u.id "
        "where f.parent_id = $1 and f.deleted_at is NULL "
        "order by f.name;";

    const char *params[1];
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);
    params[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: %s", PQerrorMessage(conn));
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);
    if ((size_t)rows > *maxlen) rows = *maxlen;

    for (int i = 0; i < rows; ++i) {
        entries[i].type = ENTRY_FOLDER;
    
        entries[i].id = atol(PQgetvalue(res, i, 0));

        snprintf(entries[i].name, sizeof(entries[i].name), "%s", PQgetvalue(res, i, 1));

        snprintf(entries[i].owner, sizeof(entries[i].owner), "%s", PQgetvalue(res, i, 2));

        snprintf(entries[i].updated_at, sizeof(entries[i].updated_at), "%s", PQgetvalue(res, i, 3));
    }

    *maxlen = rows;
    PQclear(res);
    db_release(conn);

    return OK;
}

// Tìm folder cha
db_errror_code db_folder_find_parent(int folder_id, int *id) {
    if (id == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // Kiểm tra trạng thái hiện tại của folder
    int ret = db_folder_check_status(folder_id);
    if (ret != OK) {
        return ret;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query = "select parent_id from folders where id = $1";
    
    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);

    const char *param[1];
    param[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, param, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "find parent error: %s\n", PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    // Nếu là root folder
    if (PQgetisnull(res, 0, 0)) {
        // Khong thay doi id folder
        *id = folder_id;

        PQclear(res);
        db_release(conn);

        return OK;
    }

    // Lấy parent_id
    *id = atoi(PQgetvalue(res, 0, 0));

    PQclear(res);
    db_release(conn);

    return OK;
}

// Tìm folder có tên
db_errror_code db_folder_find_folder_by_name(int folder_id, char *dirname, int *id) {
    if (dirname == NULL || id == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // Kiểm tra trạng thái hiện tại của folder
    int ret = db_folder_check_status(folder_id);
    if (ret != OK) {
        return ret;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query = "select id from folders " 
        "where parent_id = $1 and name = $2 "
        "and deleted_at is null";
    
    char id_str[32]; snprintf(id_str, sizeof(id_str), "%d", folder_id);

    const char *param[2];
    param[0] = id_str;
    param[1] = dirname;

    PGresult *res = PQexecParams(conn, query, 2, NULL, param, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
        fprintf(stderr, "Error: find folder by name: %s\n", PQerrorMessage(conn));
        
        PQclear(res);
        db_release(conn);

        return ERR;
    }

    // Không tìm thấy folder với tên đó
    if (PQntuples(res) == 0) {

        PQclear(res);
        db_release(conn);

        return DB_FOLDER_NAME_NOT_FOUND;
    }

    // Lấy id folder
    *id = atoi(PQgetvalue(res, 0, 0));
    
    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_folder_check_exist_dirname(int folder_id, char *fname, bool *flag) {
    if (fname == NULL)
        return ERR_INVALID_ARGUMENT;

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "db_acuire(): pool exhausted\n");
        return ERR;
    }

    const char *query = "select * from folders where parent_id = $1 and name = $2";
    const char *params[2];
    params[0] = id_str;
    params[1] = fname;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    // Không tìm thấy folder với tên đó
    if (PQntuples(res) == 0) {
        *flag = true;
    } else {
        *flag = false;
    }

    PQclear(res);
    db_release(conn);
    return OK;
}

db_errror_code db_folder_create_new_folder(int folder_id, int owner_id, char *fname) {
    if (fname == NULL)
        return ERR_INVALID_ARGUMENT;

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);
    char owner_str[32];
    snprintf(owner_str, sizeof(owner_str), "%d", owner_id);

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query = "insert into folders "
        "(owner_id, parent_id, name) "
        "values ($1, $2, $3); ";

    const char *params[3];
    params[0] = owner_str;
    params[1] = id_str;
    params[2] = fname;

    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "insert folder error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);
    return OK;
}

db_errror_code db_folder_is_empty(int folder_id) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT EXISTS ("
        "   SELECT 1 "
        "   FROM folders "
        "   WHERE parent_id = $1 "
        "   AND deleted_at IS NULL"
        ") "
        "OR EXISTS ("
        "   SELECT 1 "
        "   FROM files "
        "   WHERE folder_id = $1 "
        "   AND deleted_at IS NULL"
        ");";

    char folder_id_str[16];
    snprintf(folder_id_str,  sizeof(folder_id_str), "%d", folder_id);

    const char *params[1] = {
        folder_id_str
    };

    PGresult *res = PQexecParams(
        conn,
        query,
        1,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "db_folder_is_empty: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    int not_empty =
        strcmp(PQgetvalue(res, 0, 0), "t") == 0;

    PQclear(res);
    db_release(conn);

    if (not_empty) {
        return DB_FOLDER_NOT_EMPTY;
    }

    return OK;
}

db_errror_code db_folder_soft_delete(int folder_id) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "UPDATE folders "
        "SET "
        "deleted_at = NOW(), "
        "updated_at = NOW() "
        "WHERE id = $1 "
        "AND deleted_at IS NULL;";

    char folder_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", folder_id);

    const char *params[1] = {
        folder_id_str
    };

    PGresult *res = PQexecParams(
        conn,
        query,
        1,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "db_folder_soft_delete: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    if (atoi(PQcmdTuples(res)) == 0) {
        PQclear(res);
        db_release(conn);

        return DB_FOLDER_NOT_FOUND;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_folder_rename(int folder_id, const char *new_name) {
    if (new_name == NULL) {
        return ERR;
    }

    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "UPDATE folders "
        "SET name = $2, "
        "    updated_at = NOW() "
        "WHERE id = $1 "
        "AND deleted_at IS NULL;";

    char folder_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", folder_id);

    const char *params[2] = {
        folder_id_str,
        new_name
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

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

        const char *sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);

        if (sqlstate &&
            strcmp(sqlstate, "23505") == 0) {

            PQclear(res);
            db_release(conn);

            return DB_FOLDER_EXISTS;
        }

        fprintf(stderr, "db_folder_rename failed: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    if (atoi(PQcmdTuples(res)) == 0) {

        PQclear(res);
        db_release(conn);

        return DB_FOLDER_NOT_FOUND;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}