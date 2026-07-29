#include <stdio.h>

#include "../../include/app.h"
#include "../../include/env.h"
#include "../../include/database.h"
#include "../../include/db_pool.h"
#include "../../include/server.h"

#define PORT 8080

static const char *g_conninfo = NULL;

int app_init(const char *env_path) {
    g_conninfo = load_env(env_path);

    if (g_conninfo == NULL) {
        fprintf(stderr, "app_init: failed to load env\n");
        return -1;
    }

    PGconn *test = db_connect(g_conninfo);

    if (test == NULL) {
        fprintf(stderr, "app_init: DB connection failed\n");
        return -1;
    }

    db_disconnect(test);

    if (db_pool_init(g_conninfo) != 0) {
        fprintf(stderr, "app_init: pool init failed\n");
        return -1;
    }

    return 0;
}

void app_run(void) {
    start_server(PORT);
}

void app_shutdown(void) {
    db_pool_destroy();
}