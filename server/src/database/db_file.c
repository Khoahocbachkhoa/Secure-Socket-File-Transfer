#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../include/db_pool.h"
#include "../../include/db_folder.h"
#include "../../include/db_files.h"
#include "../../include/db_user.h"
#include "../../include/database.h"

db_errror_code db_file_list(int folder_id, Entry *entries, size_t *maxlen) {
    if (entries == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // int ret = db_folder_check_status(folder_id);
    // if (ret != OK) {
    //     return ret;
    // }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "List files: Db exhausted error\n");
        return ERR;
    }

    const char *query = "select f.id, f.filename, u.username as ownername, f.updated_at "
        "from files f join users u on f.owner_id = u.id "
        "where f.folder_id = $1 and f.deleted_at is NULL "
        "order by f.filename;";

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
        entries[i].type = ENTRY_FILE;
    
        entries[i].id = atol(PQgetvalue(res, i, 0));

        snprintf(entries[i].name, sizeof(entries[i].name), "%s", PQgetvalue(res, i, 1));

        snprintf(entries[i].owner, sizeof(entries[i].owner), "%s", PQgetvalue(res, i, 2));

        snprintf(entries[i].updated_at, sizeof(entries[i].updated_at), "%s", PQgetvalue(res, i, 3));
    }

    *maxlen = (size_t)rows;
    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_exists(int fid, const char *fname) {
    if (fname == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query = "select 1 "
        "from files "
        "where filename = $1 "
        "and folder_id = $2 "
        "and deleted_at is NULL";
    
    char id[32]; snprintf(id, sizeof(id), "%d", fid);
    
    const char *params[2];
    params[0] = fname;
    params[1] = id;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: %s", PQerrorMessage(conn));
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    db_errror_code ret;

    if (PQntuples(res) > 0) {
        ret = DB_FILE_EXISTS;
    } else {
        ret = DB_FILE_NOT_FOUND;
    }

    PQclear(res);
    db_release(conn);

    return ret;
}

db_errror_code db_file_insert(char *fname, char *storagekey, int fsize, session_t *session) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        return ERR;
    }

    const char *query =
        "INSERT INTO files "
        "(owner_id, folder_id, filename, storage_key, size) "
        "VALUES ($1, $2, $3, $4, $5);";

    char owner_str[32];
    char folder_str[32];
    char size_str[32];
    snprintf(owner_str, sizeof(owner_str), "%d", session->user_id);
    snprintf(folder_str, sizeof(folder_str), "%ld", session->current_folder_id);
    snprintf(size_str, sizeof(size_str), "%d", fsize);

    const char *params[5];
    params[0] = owner_str;
    params[1] = folder_str;
    params[2] = fname;
    params[3] = storagekey;
    params[4] = size_str;

    PGresult *res = PQexecParams(conn, query, 5, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        db_release(conn);

        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_find_by_name(int folder_id, char *fname, FileMeta *meta) {
    if (fname == NULL || meta == NULL)
        return ERR_INVALID_ARGUMENT;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query =
        "select id, owner_id, filename, storage_key, size "
        "from files "
        "where folder_id = $1 "
        "and filename = $2 "
        "and deleted_at is NULL;";

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);
    const char *params[2];
    params[0] = id_str;
    params[1] = fname;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);
        return DB_FILE_NOT_FOUND;
    }    

    meta->id = atoi(PQgetvalue(res, 0, 0));
    meta->owner_id = atoi(PQgetvalue(res, 0, 1));
    snprintf(meta->filename, sizeof(meta->filename), "%s", PQgetvalue(res, 0, 2));
    snprintf(meta->storage_key, sizeof(meta->storage_key), "%s", PQgetvalue(res, 0, 3));
    meta->size = atol(PQgetvalue(res, 0, 4));

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_find_by_id(int fid, FileMeta *meta) {
    if (meta == NULL)
        return ERR_INVALID_ARGUMENT;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query =
        "select id, owner_id, filename, storage_key, size "
        "from files "
        "where id = $1 "
        "and deleted_at is NULL;";

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", fid);
    const char *params[2];
    params[0] = id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);
        return DB_FILE_NOT_FOUND;
    }    

    meta->id = atoi(PQgetvalue(res, 0, 0));
    meta->owner_id = atoi(PQgetvalue(res, 0, 1));
    snprintf(meta->filename, sizeof(meta->filename), "%s", PQgetvalue(res, 0, 2));
    snprintf(meta->storage_key, sizeof(meta->storage_key), "%s", PQgetvalue(res, 0, 3));
    meta->size = atol(PQgetvalue(res, 0, 4));

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_find_id_by_name(int folder_id, char *fname, int *id) {
    if (fname == NULL)
        return ERR_INVALID_ARGUMENT;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        return ERR;
    }

    const char *query =
        "select id "
        "from files "
        "where folder_id = $1 "
        "and filename = $2 "
        "and deleted_at is NULL;";

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%d", folder_id);
    const char *params[2];
    params[0] = id_str;
    params[1] = fname;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        db_release(conn);
        return ERR;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);
        return DB_FILE_NOT_FOUND;
    }    

    *id = atoi(PQgetvalue(res, 0, 0));

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_get_storage_key_by_id(int file_id, char *storage_key, int storage_key_size) {
    if (storage_key == NULL || storage_key_size == 0) {
        return ERR;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT storage_key "
        "FROM files "
        "WHERE id = $1";

    char file_id_str[16];
    snprintf(file_id_str, sizeof(file_id_str), "%d", file_id);

    const char *params[1] = {
        file_id_str
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
        fprintf(stderr, "Failed to get storage key: %s\n", PQerrorMessage(conn));
        
        PQclear(res);
        db_release(conn);

        return ERR;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        db_release(conn);
        return DB_FILE_NOT_FOUND;
    }

    snprintf(storage_key, storage_key_size, "%s", PQgetvalue(res, 0, 0));

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_soft_delete(int file_id) {
    PGconn *conn = db_acquire();

    if (conn == NULL)
        return ERR;

    const char *query =
        "UPDATE files "
        "SET deleted_at = NOW() "
        "WHERE id = $1 "
        "AND deleted_at IS NULL;";

    char file_id_str[16];

    snprintf(file_id_str, sizeof(file_id_str), "%d", file_id);

    const char *params[1] = {
        file_id_str
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
        fprintf(stderr, "soft delete file failed: %s\n",
            PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_file_rename(int file_id, const char *new_name) {
    if (new_name == NULL) {
        return ERR;
    }

    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "UPDATE files "
        "SET filename = $2, "
        "    updated_at = NOW() "
        "WHERE id = $1 "
        "AND deleted_at IS NULL;";

    char file_id_str[16];

    snprintf(file_id_str, sizeof(file_id_str), "%d", file_id);

    const char *params[2] = {
        file_id_str,
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
        fprintf(stderr, "db_file_rename failed: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    if (atoi(PQcmdTuples(res)) == 0) {
        PQclear(res);
        db_release(conn);

        return DB_FILE_NOT_FOUND;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}