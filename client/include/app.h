#ifndef APP_H
#define APP_H

#define PORT 8080
#define SERVADDR "127.0.0.1"

// Khởi tạo ứng dụng
void init(int argc, char *argv[]);

// Prompt
void shell_print_prompt();

// Read line
char *shell_read_line();

// exec command from input
void shell_excute(char *line);

// Shutdown app
void app_shutdown();

#endif