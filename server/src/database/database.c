#include <stdio.h>
#include <stdlib.h>

#include "../../include/database.h"

const char *g_db_conninfo = NULL;

PGconn *db_connect(const char *conninfo) {
    if (conninfo == NULL) {
        fprintf(stderr, "db_connect: conninfo is NULL\n");
        return NULL;
    }

    PGconn *conn = PQconnectdb(conninfo);

    if (conn == NULL) {
       fprintf(stderr, "db_connect: PQconnectdb failed!\n");
       return NULL; 
    }

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "db_connect: Database connection failed: %s\n", PQerrorMessage(conn));

        PQfinish(conn);

        return NULL;
    }

    return conn;
}

void db_disconnect(PGconn *conn) {
    if (conn != NULL) {
        PQfinish(conn);
    }
}

db_connection_state db_is_connected(PGconn *conn) {
    if (conn == NULL)
        return DB_DISCONNECTED;

    return (PQstatus(conn) == CONNECTION_OK)
        ? DB_CONNECTED
        : DB_DISCONNECTED;
}