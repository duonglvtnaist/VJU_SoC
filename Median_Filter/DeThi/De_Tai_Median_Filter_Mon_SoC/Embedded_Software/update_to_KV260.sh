#!/bin/bash

# filepath: s:\Research\2025\DeAnCuoiKy\KV260_Software\update_to_KV260.sh

# Đường dẫn thư mục nguồn (thư mục hiện tại)
SOURCE_DIR=$(pwd)

# Thông tin máy từ xa
REMOTE_USER="debian"
REMOTE_HOST="163.221.183.90"
REMOTE_PASSWORD="temppwd"
REMOTE_DIR="/home/debian/VJU_SoC/MedianFilter/Software"

# # Kiểm tra xem lệnh sshpass đã được cài đặt chưa
# if ! command -v sshpass &> /dev/null; then
#     echo "sshpass chưa được cài đặt. Vui lòng cài đặt sshpass trước khi chạy script này."
#     echo "Cài đặt sshpass bằng lệnh: sudo apt-get install sshpass"
#     exit 1
# fi

# Thực hiện sao chép toàn bộ file từ thư mục hiện tại sang máy từ xa
echo "Đang sao chép file từ $SOURCE_DIR sang $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR ..."
sshpass -p "$REMOTE_PASSWORD" scp -r "$SOURCE_DIR"/* "$REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR"

# Kiểm tra kết quả
if [ $? -eq 0 ]; then
    echo "Sao chép thành công!"
else
    echo "Có lỗi xảy ra trong quá trình sao chép."
fi