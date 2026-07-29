#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/storage.h"
#include "../../include/transport.h"

// Ghi file ra đĩa
bool storage_save_file(const char *path, int clientfd, size_t filesize) {
    FILE *fp = fopen(path, "wb");

    if (fp == NULL) {
        return false;
    }

    char buf[4096];
    size_t remaining = filesize;
    
    while (remaining > 0) {
        // Đọc theo từng chunk
        size_t chunk = remaining < sizeof(buf) ? remaining : sizeof(buf);
        
        // Đọc đủ chunk
        size_t n = net_recv_all(clientfd, buf, chunk, 0);

        // Nếu có lỗi hoặc client đóng kết nối
        if (n <= 0) {
            fclose(fp);
            return false;
        }

        fwrite(buf, 1, n, fp);
        remaining -= n;
    }

    fclose(fp);
    return true;
}

bool storage_send_file(const char *path, int clientfd, size_t filesize) {
    FILE *fp = fopen(path, "rb");

    if (fp == NULL) {
        return false;
    }

    char buf[4096];
    size_t remaining = filesize;

    while (remaining > 0) {
        // Đọc file theo từng chunk
        size_t chunk = (remaining < sizeof(buf)) ? remaining : sizeof(buf);

        size_t nread = fread(buf, 1, chunk, fp);

        // Lỗi khi đọc file
        if (nread == 0) {
            fclose(fp);
            return ERR;
        }

        size_t n = net_send_all(clientfd, buf, nread, 0);

        // Lỗi khi gửi hoặc client đóng kết nối
        if (n <= 0) {
            fclose(fp);
            return false;
        }

        remaining -= nread;
    }

    fclose(fp);
    return true;
}

// Not implement yet
// bool storage_delete_file(const char *path) {
//     return true;
// }