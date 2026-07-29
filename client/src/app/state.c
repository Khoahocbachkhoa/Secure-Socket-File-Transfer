#include <stdio.h>
#include <arpa/inet.h>

#include "../../include/state.h"

ClientState global_state;
ClientState *state = &global_state;

void init_state(ClientState *state, int fd) {
    snprintf(state->username, sizeof(state->username), "guest");
    snprintf(state->cwd, sizeof(state->cwd), "-");
    state->is_logged_in = false;
    state->sockfd = fd;
}