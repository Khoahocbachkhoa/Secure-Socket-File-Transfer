#include "../include/app.h"

int main(void) {
    if (app_init("./.env") != 0) {
        return 1;
    }

    app_run();

    app_shutdown();

    return 0;
}