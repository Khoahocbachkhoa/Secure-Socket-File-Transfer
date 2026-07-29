#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "../../include/security.h"

password_status check_password(const char *pass) {
    if (pass == NULL) {
        return PASSWORD_INVALID_ARGUMENT;
    }

    int has_upper = 0;
    int has_lower = 0;
    int has_digit = 0;
    int has_special = 0;

    int len = strlen(pass);

    // Ít nhất 10 ký tự
    if (len < 10) {
        return PASSWORD_WEAK;
    }

    for (int i = 0; pass[i] != '\0'; i++) {
        unsigned char c = pass[i];

        if (isupper(c)) {
            has_upper = 1;
        }
        else if (islower(c)) {
            has_lower = 1;
        }
        else if (isdigit(c)) {
            has_digit = 1;
        }
        else {
            has_special = 1;
        }
    }

    return (has_upper && has_lower && has_digit && has_special) ? PASSWORD_STRONG : PASSWORD_WEAK;
}

int password_hash(const char *password, char hash_hex[65]) {
    unsigned char digest[SHA256_DIGEST_LENGTH];

    SHA256(
        (const unsigned char *)password,
        strlen(password),
        digest
    );

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_hex + (i * 2), "%02x", digest[i]);
    }

    hash_hex[64] = '\0';

    return 0;
}