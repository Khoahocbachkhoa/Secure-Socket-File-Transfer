#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/db_pool.h"
#include "../../include/database.h"
#include "../../include/db_sharing.h"
#include "../../include/db_folder.h"

db_errror_code db_sharing_grant_file_access(int file_id, int user_id) {
    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "File access: Db exhausted error\n");
        return ERR;
    }

    const char *query =
        "INSERT INTO file_permissions "
        "(file_id, user_id) "
        "VALUES ($1, $2)";

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

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr,
                "Failed to grant file access: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

// hủy quyền chia sẻ file cho 1 user
db_errror_code db_sharing_revoke_file_access(int file_id, int user_id) {
    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "File access: Db exhausted error\n");
        return ERR;
    }

    const char *query =
        "DELETE FROM file_permissions "
        "WHERE file_id = $1 AND user_id = $2";

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

    char *affected = PQcmdTuples(res);

    if (affected != NULL && strcmp(affected, "0") == 0) {
        fprintf(stderr,
                "No permission found for file_id=%d user_id=%d\n",
                file_id, user_id);

        PQclear(res);
        db_release(conn);
        return DB_SHARING_NOT_FOUND;
    }

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr,
                "Failed to revoke file access: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_list_users_shared(int file_id, char **users, int *users_size) {
    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT u.username "
        "FROM file_permissions fp "
        "JOIN users u ON fp.user_id = u.id "
        "WHERE fp.file_id = $1 "
        "ORDER BY u.username";

    char file_id_str[16];
    snprintf(file_id_str, sizeof(file_id_str), "%d", file_id);

    const char *params[1] = { file_id_str };

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
        fprintf(stderr,
                "Failed to list shared users: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);

    if (*users_size < rows) {
        // Khong du kich thuoc bo dem
        rows = *users_size;
    }

    *users_size = rows;

    if (rows == 0) {
        PQclear(res);
        db_release(conn);
        return OK;
    }

    for (int i = 0; i < rows; i++) {
        const char *username = PQgetvalue(res, i, 0);

        //users[i] = strdup(username);
        users[i] = malloc(256 * sizeof(char));
        memcpy(users[i], username, 256);
        if (users[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(users[j]);
            }

            PQclear(res);
            db_release(conn);
            return ERR;
        }
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_list_files_shared_with_me(int user_id, Entry *entries, int *len) {
    if (entries == NULL || len == NULL) {
        return ERR;
    }

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT "
        "f.id, "
        "f.filename, "
        "u.username, "
        "f.updated_at "
        "FROM file_permissions fp "
        "JOIN files f ON fp.file_id = f.id "
        "JOIN users u ON f.owner_id = u.id "
        "WHERE fp.user_id = $1 "
        "ORDER BY f.filename";

    char user_id_str[16];
    snprintf(user_id_str, sizeof(user_id_str),
             "%d", user_id);

    const char *params[1] = {
        user_id_str
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
        fprintf(stderr,
                "Failed to list shared files: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);

    if (rows > *len) {
        fprintf(stderr,
                "Buffer too small (%d < %d)\n",
                *len, rows);

        *len = rows;

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    for (int i = 0; i < rows; i++) {
        entries[i].id = atol(PQgetvalue(res, i, 0));

        entries[i].type = ENTRY_FILE;

        snprintf(entries[i].name, sizeof(entries[i].name), "%s", PQgetvalue(res, i, 1));

        snprintf(entries[i].owner, sizeof(entries[i].owner), "%s",
                PQgetvalue(res, i, 2));

        snprintf(entries[i].updated_at, sizeof(entries[i].updated_at), "%s",
                PQgetvalue(res, i, 3));
    }

    *len = rows;

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_grant_folder_access(int folder_id, int user_id, char *role) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr,
                "Folder access: Db exhausted error\n");
        return ERR;
    }

    const char *query =
        "INSERT INTO folder_permissions "
        "(folder_id, user_id, role) "
        "VALUES ($1, $2, $3)";

    char folder_id_str[16];
    char user_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", folder_id);

    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[3] = {
        folder_id_str,
        user_id_str,
        role
    };

    PGresult *res = PQexecParams(
        conn,
        query,
        3,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to grant folder access: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_revoke_folder_access(
    int folder_id,
    int user_id
) {
    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr,
                "Folder revoke: Db exhausted error\n");
        return ERR;
    }

    const char *query =
        "DELETE FROM folder_permissions "
        "WHERE folder_id = $1 "
        "AND user_id = $2;";

    char folder_id_str[16];
    char user_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", folder_id);

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

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to revoke folder access: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    // Nếu không tìm thấy đã chia sẻ
    if (strcmp(PQcmdTuples(res), "0") == 0) {
        PQclear(res);
        db_release(conn);
        return DB_SHARING_NOT_FOUND;
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_list_users_shared_folder( int folder_id, shared_user_t *users, int *user_size) {
    if (users == NULL || user_size == NULL)
        return ERR;

    PGconn *conn = db_acquire();
    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT u.username, fp.role "
        "FROM folder_permissions fp "
        "JOIN users u ON fp.user_id = u.id "
        "WHERE fp.folder_id = $1 "
        "ORDER BY u.username;";

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

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to list shared folder users: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);
        return ERR;
    }

    int rows = PQntuples(res);

    if (rows > *user_size) {
        rows = *user_size;
    }

    *user_size = rows;

    for (int i = 0; i < rows; i++) {
        const char *username = PQgetvalue(res, i, 0);
        const char *role     = PQgetvalue(res, i, 1);

        snprintf(users[i].username, sizeof(users[i].username), "%s", username);

        snprintf(users[i].role, sizeof(users[i].role), "%s", role);
    }

    PQclear(res);
    db_release(conn);

    return OK;
}

db_errror_code db_sharing_list_folders_shared_with_me( int user_id, Entry *entries, int *len) {
    if (entries == NULL || len == NULL) {
        return ERR;
    }

    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT "
        "f.id, "
        "f.name, "
        "u.username, "
        "fp.role, "
        "f.updated_at "
        "FROM folder_permissions fp "
        "JOIN folders f ON fp.folder_id = f.id "
        "JOIN users u ON f.owner_id = u.id "
        "WHERE fp.user_id = $1 "
        "AND f.deleted_at IS NULL "
        "ORDER BY f.name;";

    char user_id_str[16];

    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[1] = {
        user_id_str
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
        fprintf(stderr, "Failed to list shared folders: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    int rows = PQntuples(res);

    if (rows > *len) {
        fprintf(stderr, "Buffer too small (%d < %d)\n", *len, rows);

        *len = rows;

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    for (int i = 0; i < rows; i++) {

            entries[i].id = atol(PQgetvalue(res, i, 0));

            entries[i].type = ENTRY_FOLDER;

            snprintf(entries[i].name, sizeof(entries[i].name), "%s", PQgetvalue(res, i, 1));

            snprintf(entries[i].owner, sizeof(entries[i].owner), "%s",
                    PQgetvalue(res, i, 2));

            snprintf(entries[i].role, sizeof(entries[i].role), "%s",
                    PQgetvalue(res, i, 3));

            snprintf(entries[i].updated_at, sizeof(entries[i].updated_at), "%s",
                    PQgetvalue(res, i, 4));
        }

    *len = rows;

    PQclear(res);
    db_release(conn);

    return OK;
}

// typedef struct {
//     int owner_id;
//     char owner_name[256];
//     char role[16];
// } access_info_t;

db_errror_code db_sharing_get_folder_access_info(int folder_id, access_info_t *info, session_t *session) {
    if (info == NULL || session == NULL) {
        return ERR;
    }

    PGconn *conn = db_acquire();

    if (conn == NULL) {
        fprintf(stderr, "DB exhausted error\n");
        return ERR;
    }

    const char *query =
        "SELECT "
        "u.id, "
        "u.username, "
        "fp.role, "
        "f.name "
        "FROM folder_permissions fp "
        "JOIN folders f "
        "ON fp.folder_id = f.id "
        "JOIN users u "
        "ON f.owner_id = u.id "
        "WHERE fp.folder_id = $1 "
        "AND fp.user_id = $2;";

    char folder_id_str[16];
    char user_id_str[16];

    snprintf(folder_id_str, sizeof(folder_id_str), "%d", folder_id);

    snprintf(user_id_str, sizeof(user_id_str), "%d", session->user_id);

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

        fprintf(stderr, "db_sharing_get_folder_access_info: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        db_release(conn);

        return ERR;
    }

    if (PQntuples(res) == 0) {

        PQclear(res);
        db_release(conn);

        return DB_PERMISSION_DENIED;
    }

    info->owner_id = atoi(PQgetvalue(res, 0, 0));

    snprintf(info->owner_name, sizeof(info->owner_name), "%s",
             PQgetvalue(res, 0, 1));

    snprintf(info->role, sizeof(info->role), "%s", PQgetvalue(res, 0, 2));

    snprintf(info->folder_name, sizeof(info->folder_name), "%s", PQgetvalue(res, 0, 3));

    PQclear(res);
    db_release(conn);

    return OK;
}