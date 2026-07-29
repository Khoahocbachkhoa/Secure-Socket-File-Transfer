#ifndef APP_H
#define APP_H

// Khởi tạo app (đọc biến môi trường, kiểm tra kết nối server)
int app_init(const char *env_path);

// Chạy app
void app_run(void);

// Giải phóng tài nguyên và đóng kết nối
void app_shutdown(void);

#endif