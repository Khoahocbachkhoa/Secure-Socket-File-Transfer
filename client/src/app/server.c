#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../include/server.h"

int connect_to_server(char *ip, int port) {
    int sockfd;
    // connect to server as an guest
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Cannot create socket!\n");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "Cannot connect to server!\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}
