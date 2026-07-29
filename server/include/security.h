#ifndef SECURITY_H
#define SECURITY_H

typedef enum {
    PASSWORD_INVALID_ARGUMENT = 0,
    PASSWORD_WEAK,
    PASSWORD_STRONG
} password_status;


// Kiểm tra độ mạnh của một mật khẩu
password_status check_password(const char *pass);

// Băm mật khẩu
int password_hash(const char *password, char hash_hex[65]);

#endif