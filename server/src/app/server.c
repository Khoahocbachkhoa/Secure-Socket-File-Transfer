#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "../../include/server.h"
#include "../../include/client.h"
#include "../../include/tls_server.h"

#define LISTENQ 100

void start_server(int port) {
    pthread_t tid;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Tạo socket lắng nghe kết nối từ client 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("server.c: Lỗi tạo socket!");
        exit(EXIT_FAILURE);
    }

    // Reuse socket, tránh trường hợp socket không khả dụng!
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("server.c: setsockopt error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // bind socket
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("server.c: bind error!");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(sockfd, LISTENQ) < 0) {
        perror("server.c: listen error!");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    // tạo context tls
    SSL_CTX *ctx = tls_server_ctx_create();

    if (ctx == NULL) {
        fprintf(stderr, "Cannot create TLS context\n");
        exit(EXIT_FAILURE);
    }

    // luồng mới xử lý client
    while (1) {
        clilen = sizeof(cliaddr);
        ClientContext *ctx_client = malloc(sizeof(ClientContext));

        if (ctx_client == NULL) {
            perror("malloc");
            continue;
        }

        ctx_client->clientfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);

        if (ctx_client->clientfd < 0) {
            perror("accept");
            continue;
        }

        ctx_client->ssl = tls_accept(ctx, ctx_client->clientfd);
        
        if (ctx_client->ssl == NULL) {
            printf("TLS handshake failed\n");

            close(ctx_client->clientfd);
            free(ctx_client);

            continue;
        }

        g_ssl_table[ctx_client->clientfd] = ctx_client->ssl;

        // In ra client kết nối
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
        printf("Client connected: %s:%d\n", ip, ntohs(cliaddr.sin_port));

        int ret = pthread_create(&tid, NULL, client_thread, ctx_client);
        if (ret != 0) {
            fprintf(stderr, "server.c: pthread_create error: %s\n", strerror(ret));

            close(ctx_client->clientfd);
            free(ctx_client);

            continue;
        }

        pthread_detach(tid);
    }

    SSL_CTX_free(ctx);
}

void *client_thread(void *arg) {
    ClientContext *ctx = arg;

    handle_client(ctx->clientfd, ctx->ssl);

    SSL_shutdown(ctx->ssl);
    SSL_free(ctx->ssl);

    close(ctx->clientfd);

    g_ssl_table[ctx->clientfd] = NULL;

    free(ctx);

    return NULL;
}