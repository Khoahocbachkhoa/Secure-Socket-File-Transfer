# Secure File Sharing System

## Giới thiệu

Secure File Sharing System là ứng dụng chia sẻ tệp tin theo mô hình Client-Server được xây dựng bằng ngôn ngữ C.

Hệ thống hỗ trợ:

* Đăng ký và đăng nhập tài khoản.
* Quản lý thư mục và tệp tin.
* Upload và download tệp tin.
* Chia sẻ tệp tin và thư mục.
* Phân quyền trong chế độ chia sẻ.
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

Khởi tạo cơ sở dữ liệu với schema trong /server/src/database/schema.sql

Thiết lập kênh truyền TLS bằng OpenSSL

Tạo file môi trường .env và đặt biến kết nối Database

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

### Hệ thống

```text
help
quit
```

---

## Bảo mật

Hệ thống sử dụng TLS thông qua thư viện OpenSSL.

Server sử dụng chứng chỉ số được ký bởi Root CA tự tạo.

Client xác thực chứng chỉ của Server trước khi thiết lập phiên làm việc.

Toàn bộ dữ liệu đăng nhập, lệnh điều khiển và nội dung tệp được truyền qua kênh mã hóa TLS.