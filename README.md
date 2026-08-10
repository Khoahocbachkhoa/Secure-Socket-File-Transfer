# Secure File Sharing System

## Giới thiệu

Secure File Sharing System là chương trình chia sẻ tệp tin được xây dựng bằng ngôn ngữ C.

Chương trình hỗ trợ:

* Đăng ký, đăng nhập tài khoản.
* Quản lý, lưu trữ thư mục tập tin trên server.
* Upload và download.
* Chia sẻ tệp tin, thư mục giữa các tài khoản.
* Phân quyền truy cập tài nguyên khi chia sẻ.
* Mã hóa kênh truyền và xác thực.

---

## Yêu cầu

### Server

* Linux
* GCC
* PostgreSQL
* OpenSSL

### Client

* Linux
* GCC
* OpenSSL

---

## Chạy chương trình

### Khởi tạo

Khởi tạo CSDL với schema trong /server/src/database/schema.sql

Tạo chứng chỉ, thiết lập kênh TLS bằng OpenSSL.

Tạo kết nối server với database.

### Biên dịch

```bash
make
```

### Khởi động server

```bash
./server.out <port>
```

### Kết nối client

```bash
./client.out <serv_addr> <port>
```

---

## Các lệnh hỗ trợ

### Xác thực

```text
register
login
logout
```

### Quản lý thư mục

```text
pwd
ls
cd
mkdir
rmdir
mv
rm
```

### Truyền tệp

```text
put
get
```

### Chia sẻ

```text
share-file
unshare-file

share-folder
unshare-folder

file-acl
folder-acl

shared-files
shared-folders

get-shared

open-shared
exit-shared
```

### Khác

```text
help
quit
```