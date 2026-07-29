#include <stdio.h>
#include <stdlib.h>

#include "../../include/db_pool.h"
#include "../../include/database.h"

static db_pool_t g_pool;

// Khởi tạo pool
int db_pool_init(const char *conninfo) {
    g_pool.conninfo = conninfo;

    pthread_mutex_init(&g_pool.mutex, NULL);

    for (int i = 0; i < DB_POOL_SIZE; i++) {
        PGconn *c = db_connect(conninfo);

        if (c == NULL) {
            fprintf(stderr, "db_pool.c: DB init failed!\n");
            return EXIT_FAILURE;
        }

        g_pool.pool[i].conn = c;
        g_pool.pool[i].used = 0;
    }

    return EXIT_SUCCESS;
}

// Xin 1 pool
PGconn* db_acquire(void) {
    pthread_mutex_lock(&g_pool.mutex);

    for (int i = 0; i < DB_POOL_SIZE; i++) {
        if (g_pool.pool[i].used == 0) {
            g_pool.pool[i].used = 1;

            pthread_mutex_unlock(&g_pool.mutex);
            return g_pool.pool[i].conn;
        }
    }

    pthread_mutex_unlock(&g_pool.mutex);
    return NULL;
}

// Release 1 pool
void db_release(PGconn *conn) {
    pthread_mutex_lock(&g_pool.mutex);

    for (int i = 0; i < DB_POOL_SIZE; i++) {
        if (g_pool.pool[i].conn == conn) {
            g_pool.pool[i].used = 0;
            break;
        }
    }

    pthread_mutex_unlock(&g_pool.mutex);
}

// Kết thúc tất cả connection
void db_pool_destroy(void) {
    for (int i = 0; i < DB_POOL_SIZE; i++) {
        if (g_pool.pool[i].conn) {
            db_disconnect(g_pool.pool[i].conn);
        }
    }

    pthread_mutex_destroy(&g_pool.mutex);
}