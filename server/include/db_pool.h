#ifndef DB_POOL_H
#define DB_POOL_H

#include <libpq-fe.h>
#include "database.h"
#include <pthread.h>

#define DB_POOL_SIZE 16

typedef struct {
    PGconn *conn;
    int used;
} db_conn_t;

typedef struct {
    db_conn_t pool[DB_POOL_SIZE];
    pthread_mutex_t mutex;
    const char *conninfo;
} db_pool_t;

// Khởi tạo pool
int db_pool_init(const char *conninfo);

void db_pool_destroy(void);

// Xin 1 pool để truy vấn
PGconn* db_acquire(void);

// Release 1 pool
void db_release(PGconn *pg);

#endif